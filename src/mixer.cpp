
#include "mixer.hpp"

const float MixerChannel::maxDecibels = 6.0f;
const float MixerChannel::minDecibels = Amplifier::minDecibels;
const float MixerChannel::levelSlewTimeMS = 5.0f;

void MixerChannel::setSampleRate(float sampleRate) {
	_levelSL.setParams(sampleRate, levelSlewTimeMS, maxDecibels - minDecibels);
	_rms.setSampleRate(sampleRate);
}

void MixerChannel::reset() {
	out = rms = 0.0f;
}

void MixerChannel::next(float sample, bool solo, int c) {
	float mute = _muteParam.getValue();
	if (_muteInput) {
		mute += clamp(_muteInput->getPolyVoltage(c), 0.0f, 10.0f);
	}
	bool muted = solo ? mute < 2.0f : mute > 0.5f;
	if (muted) {
		_amplifier.setLevel(_levelSL.next(minDecibels));
	}
	else {
		float level = clamp(_levelParam.getValue(), 0.0f, 1.0f);
		if (_levelInput.isConnected()) {
			level *= clamp(_levelInput.getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
		}
		level *= maxDecibels - minDecibels;
		level += minDecibels;
		_amplifier.setLevel(_levelSL.next(level));
	}

	out = _amplifier.next(sample);
	rms = _rms.next(out / 5.0f);
}


#define DIM_DB "dim_decibels"

json_t* DimmableMixerModule::toJson(json_t* root) {
	json_object_set_new(root, DIM_DB, json_real(_dimDb));
	return root;
}

void DimmableMixerModule::fromJson(json_t* root) {
	json_t* o = json_object_get(root, DIM_DB);
	if (o) {
		_dimDb = json_real_value(o);
	}
}


void DimmableMixerWidget::contextMenu(Menu* menu) {
	auto m = dynamic_cast<DimmableMixerModule*>(module);
	assert(m);
	OptionsMenuItem* da = new OptionsMenuItem("Dim amount");
	da->addItem(OptionMenuItem("-6 dB", [m]() { return m->_dimDb == 6.0f; }, [m]() { m->_dimDb = 6.0f; }));
	da->addItem(OptionMenuItem("-12 dB", [m]() { return m->_dimDb == 12.0f; }, [m]() { m->_dimDb = 12.0f; }));
	da->addItem(OptionMenuItem("-18 dB", [m]() { return m->_dimDb == 18.0f; }, [m]() { m->_dimDb = 18.0f; }));
	da->addItem(OptionMenuItem("-24 dB", [m]() { return m->_dimDb == 24.0f; }, [m]() { m->_dimDb = 24.0f; }));
	OptionsMenuItem::addToMenu(da, menu);
}


void MuteButton::randomize() {
	if (_randomize) {
		ToggleButton::randomize();
	}
}


SoloMuteButton::SoloMuteButton() {
	shadow = new CircularShadow();
	addChild(shadow);

	_svgWidget = new SvgWidget();
	addChild(_svgWidget);

	auto svg = APP->window->loadSvg(asset::plugin(pluginInstance, "res/button_18px_0.svg"));
	_frames.push_back(svg);
	_frames.push_back(APP->window->loadSvg(asset::plugin(pluginInstance, "res/button_18px_1_orange.svg")));
	_frames.push_back(APP->window->loadSvg(asset::plugin(pluginInstance, "res/button_18px_1_green.svg")));
	_frames.push_back(APP->window->loadSvg(asset::plugin(pluginInstance, "res/button_18px_1_green.svg")));

	_svgWidget->setSvg(svg);
	box.size = _svgWidget->box.size;
	shadow->box.size = _svgWidget->box.size;
	shadow->blurRadius = 1.0;
	shadow->box.pos = Vec(0.0, 1.0);
}

void SoloMuteButton::reset() {
	// if (paramQuantity) {
		setValue(0.0f);
	// }
}

void SoloMuteButton::randomize() {
	// if (paramQuantity) {
		setValue(random::uniform() > 0.5f ? 1.0f : 0.0f);
	// }
}

void SoloMuteButton::onDragStart(event::DragStart& e) {
	float value = getValue();
	if (value >= 2.0f) {
		setValue(value - 2.0f);
	}
	else if (api0::windowIsShiftPressed()) {
		setValue(value + 2.0f);
	}
	else {
		setValue(value > 0.5f ? 0.0f : 1.0f);
	}
}

void SoloMuteButton::onChange(event::Change& e) {
	assert(_frames.size() == 4);
	// if (paramQuantity) {
		float value = getValue();
		assert(value >= 0.0f && value <= 3.0f);
		_svgWidget->setSvg(_frames[(int)value]);
	// }
	ParamWidget::onChange(e);
}
