
#include <assert.h>
#include <algorithm>

#include "signal.hpp"

using namespace bogaudio::dsp;

const float Amplifier::minDecibels = -60.0f;
const float Amplifier::maxDecibels = 20.0f;
const float Amplifier::decibelsRange = maxDecibels - minDecibels;

void Amplifier::LevelTable::_generate() {
	const float rdb = 6.0f;
	const float tdb = Amplifier::minDecibels + rdb;
	const float ta = decibelsToAmplitude(tdb);
	_table[0] = 0.0f;
	for (int i = 1; i < _length; ++i) {
		float db = Amplifier::minDecibels + (i / (float)_length) * Amplifier::decibelsRange;
		if (db <= tdb) {
			_table[i] = ((db - minDecibels) / rdb) * ta;
		}
		else {
			_table[i] = decibelsToAmplitude(db);
		}
	}
}

void Amplifier::setLevel(float db) {
	if (_db != db) {
		_db = db;
		if (_db > minDecibels) {
			if (_db < maxDecibels) {
				_level = _table.value(((_db - minDecibels) / decibelsRange) * _table.length());
			}
			else {
				_level = decibelsToAmplitude(_db);
			}
		}
		else {
			_level = 0.0f;
		}
	}
}

float Amplifier::next(float s) {
	return _level * s;
}


void RootMeanSquare::setSampleRate(float sampleRate) {
	assert(sampleRate > 0.0f);
	if (_sampleRate != sampleRate) {
		_sampleRate = sampleRate;
		if (_buffer) {
			delete[] _buffer;
		}
		_bufferN = (_maxDelayMS / 1000.0f) * _sampleRate;
		_buffer = new float[_bufferN] {};
		if (_initialized) {
			_initialized = false;
			setSensitivity(_sensitivity);
		}
	}
}

void RootMeanSquare::setSensitivity(float sensitivity) {
	assert(sensitivity >= 0.0f);
	assert(sensitivity <= 1.0f);
	if (_initialized) {
		if (_sensitivity != sensitivity) {
			_sensitivity = sensitivity;
			int newSumN = std::max(_sensitivity * _bufferN, 1.0f);
			int i = newSumN;
			while (i > _sumN) {
				--_trailI;
				if (_trailI < 0) {
					_trailI = _bufferN - 1;
				}
				_sum += _buffer[_trailI];
				--i;
			}
			while (i < _sumN) {
				_sum -= _buffer[_trailI];
				++_trailI;
				_trailI %= _bufferN;
				++i;
			}
			_sumN = newSumN;
		}
	}
	else {
		_initialized = true;
		_sensitivity = sensitivity;
		_sumN = std::max(_sensitivity * _bufferN, 1.0f);
		_leadI = 0;
		_trailI = _bufferN - _sumN;
		_sum = 0.0f;
	}
}

float RootMeanSquare::next(float sample) {
	_sum -= _buffer[_trailI];
	++_trailI;
	_trailI %= _bufferN;
	_sum += _buffer[_leadI] = sample * sample;
	++_leadI;
	_leadI %= _bufferN;
	if (_sum <= 0.0) {
		return 0.0f;
	}
	return sqrtf((float)_sum / (float)_sumN);
}


void PucketteEnvelopeFollower::setSensitivity(float sensitivity) {
	assert(sensitivity >= 0.0f);
	assert(sensitivity <= 1.0f);
	_sensitivity = std::min(sensitivity, 0.97f);
}

float PucketteEnvelopeFollower::next(float sample) {
	const float norm = 5.0f;
	sample /= norm;
	_lastOutput = _sensitivity * _lastOutput + (1 - _sensitivity) * sample * sample;
	return _lastOutput * norm;
}


bool PositiveZeroCrossing::next(float sample) {
	switch (_state) {
		case NEGATIVE_STATE: {
			if (sample > positiveThreshold) {
				_state = POSITIVE_STATE;
				return true;
			}
			break;
		}
		case POSITIVE_STATE: {
			if (sample < negativeThreshold) {
				_state = NEGATIVE_STATE;
			}
			else if (sample < positiveThreshold && _triggerable) {
				_state = COUNT_ZEROES_STATE;
				_zeroCount = 1;
			}
			break;
		}
		case COUNT_ZEROES_STATE: {
			if (sample >= negativeThreshold) {
				if (++_zeroCount >= zeroesForReset) {
					_state = NEGATIVE_STATE;
				}
			}
			else {
				_state = NEGATIVE_STATE;
			}
			break;
		}
	}
	return false;
}

void PositiveZeroCrossing::reset() {
	_state = NEGATIVE_STATE;
}


void SlewLimiter::setParams(float sampleRate, float milliseconds) {
	assert(sampleRate > 0.0f);
	assert(milliseconds >= 0.0f);
	_sampleRate = sampleRate;
	_milliseconds = milliseconds;
	_samples = (_milliseconds / 1000.0f) * _sampleRate;
}

float SlewLimiter::next(float sample) {
	if (_samples < 2 || (sample > _current - 0.01f && sample < _current + 0.01f)) {
		_current = sample;
		return sample;
	}
	if (_target != sample) {
		_target = sample;
		_delta = (sample - _current) / _samples;
		_steps = _samples;
	}
	else if (_steps <= 1) {
		_current = sample;
		return sample;
	}
	_current += _delta;
	--_steps;
	return _current;
}


void CrossFader::setParams(float mix, float curve, bool linear) {
	assert(mix >= -1.0f && mix <= 1.0f);
	assert(curve >= -1.0f && curve <= 1.0f);
	if (_mix != mix || _curve != curve || _linear != linear) {
		_mix = mix;
		_curve = curve;
		_linear = linear;

		float aMax, aMin;
		float bMax, bMin;
		if (_curve < 0.0f) {
			aMax = 0.0f;
			aMin = _curve + 2.0f;
			bMax = 2.0f;
			bMin = 0.0f - _curve;
		}
		else {
			aMax = _curve;
			aMin = 2.0f;
			bMax = 2.0f - _curve;
			bMin = 0.0f;
		}

		float m = _mix + 1.0f;
		if (m < aMax) {
			_aMix = 1.0f;
		}
		else if (m > aMin) {
			_aMix = 0.0f;
		}
		else {
			_aMix = 1.0f - ((m - aMax) / (aMin - aMax));
		}

		if (m > bMax) {
			_bMix = 1.0f;
		}
		else if (m < bMin) {
			_bMix = 0.0f;
		}
		else {
			_bMix = (m - bMin) / (bMax - bMin);
		}

		if (!_linear) {
			_aAmp.setLevel((1.0f - _aMix) * Amplifier::minDecibels);
			_bAmp.setLevel((1.0f - _bMix) * Amplifier::minDecibels);
		}
	}
}

float CrossFader::next(float a, float b) {
	if (_linear) {
		return _aMix * a + _bMix * b;
	}
	return _aAmp.next(a) + _bAmp.next(b);
}


void Panner::setPan(float pan) {
	assert(pan >= -1.0f);
	assert(pan <= 1.0f);
	if (_pan != pan) {
		_pan = pan;
		_lLevel = _sineTable.value(((1.0f + _pan) / 8.0f + 0.25f) * _sineTable.length());
		_rLevel = _sineTable.value(((1.0f + _pan) / 8.0f) * _sineTable.length());
	}
}

void Panner::next(float sample, float& l, float& r) {
	l = _lLevel * sample;
	r = _rLevel * sample;
}


void DelayLine::setSampleRate(float sampleRate) {
	assert(sampleRate > 0.0f);
	if (_sampleRate != sampleRate) {
		_sampleRate = sampleRate;
		if (_buffer) {
			delete[] _buffer;
		}
		_bufferN = ceil((_maxTimeMS / 1000.0f) * _sampleRate);
		_buffer = new float[_bufferN] {};
		if (_initialized) {
			_initialized = false;
			setTime(_time);
		}
	}
}

void DelayLine::setTime(float time) {
	assert(time >= 0.0f);
	assert(time <= 1.0f);
	if (_initialized) {
		if (_time != time) {
			_time = time;
			int newDelaySamples = delaySamples();
			int i = newDelaySamples;
			while (i > _delaySamples) {
				--_trailI;
				if (_trailI < 0) {
					_trailI = _bufferN - 1;
				}
				--i;
			}
			while (i < _delaySamples) {
				++_trailI;
				_trailI %= _bufferN;
				++i;
			}
			_delaySamples = newDelaySamples;
		}
	}
	else {
		_initialized = true;
		_time = time;
		_delaySamples = delaySamples();
		_leadI = 0;
		_trailI = _bufferN - _delaySamples;
	}
}

float DelayLine::next(float sample) {
	float delayed = _buffer[_trailI];
	++_trailI;
	_trailI %= _bufferN;
	_buffer[_leadI] = sample;
	++_leadI;
	_leadI %= _bufferN;
	return delayed;
}

int DelayLine::delaySamples() {
	return std::max((_time * _maxTimeMS / 1000.0f) * _sampleRate, 1.0f);
}
