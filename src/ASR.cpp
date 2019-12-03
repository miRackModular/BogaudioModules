
#include "ASR.hpp"

#define INVERT "invert"

void ASR::Engine::reset() {
	trigger.reset();
	eocPulseGen.process(10.0);
	envelope.reset();
	on = false;
}

void ASR::Engine::sampleRateChange() {
	float sr = APP->engine->getSampleRate();
	envelope.setSampleRate(sr);
	attackSL.setParams(sr / (float)modulationSteps);
	releaseSL.setParams(sr / (float)modulationSteps);
}

void ASR::reset() {
	for (int c = 0; c < _channels; ++c) {
		_engines[c]->reset();
	}
}

void ASR::sampleRateChange() {
	for (int c = 0; c < _channels; ++c) {
		_engines[c]->sampleRateChange();
	}
}

json_t* ASR::dataToJson() {
	json_t* root = json_object();
	json_object_set_new(root, INVERT, json_real(_invert));
	return root;
}

void ASR::dataFromJson(json_t* root) {
	json_t* i = json_object_get(root, INVERT);
	if (i) {
		_invert = json_real_value(i);
	}
}

bool ASR::active() {
	return inputs[TRIGGER_INPUT].isConnected() || outputs[ENV_OUTPUT].isConnected() || outputs[EOC_OUTPUT].isConnected();
}

int ASR::channels() {
	return std::max(1, inputs[TRIGGER_INPUT].getChannels());
}

void ASR::addChannel(int c) {
	_engines[c] = new Engine(_modulationSteps);
	_engines[c]->reset();
	_engines[c]->sampleRateChange();
}

void ASR::removeChannel(int c) {
	delete _engines[c];
	_engines[c] = NULL;
}

void ASR::modulateChannel(int c) {
	Engine& e = *_engines[c];

	float attack = powf(params[ATTACK_PARAM].getValue(), 2.0f);
	if (inputs[ATTACK_INPUT].isConnected()) {
		attack *= clamp(inputs[ATTACK_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}
	e.envelope.setAttack(e.attackSL.next(attack * 10.f));

	float release = powf(params[RELEASE_PARAM].getValue(), 2.0f);
	if (inputs[RELEASE_INPUT].isConnected()) {
		release *= clamp(inputs[RELEASE_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}
	e.envelope.setRelease(e.releaseSL.next(release * 10.f));

	e.envelope.setLinearShape(_linearMode);

	_linearMode = params[LINEAR_PARAM].getValue() > 0.5f;
}

void ASR::always(const ProcessArgs& args) {
	_attackLightSum = _releaseLightSum = 0;
}

void ASR::processChannel(const ProcessArgs& args, int c) {
	Engine& e = *_engines[c];

	bool start = e.trigger.process(inputs[TRIGGER_INPUT].getVoltage(c));
	if (!e.on && start) {
		e.on = true;
	}
	e.envelope.setGate(e.trigger.isHigh() && !e.envelope.isStage(ADSR::RELEASE_STAGE));
	outputs[ENV_OUTPUT].setChannels(_channels);
	outputs[ENV_OUTPUT].setVoltage(e.envelope.next() * 10.0f * _invert * params[SUSTAIN_PARAM].getValue(), c);
	if (e.on && e.envelope.isStage(ADSR::STOPPED_STAGE)) {
		e.envelope.reset();
		e.on = false;
		e.eocPulseGen.trigger(0.001f);
	}
	outputs[EOC_OUTPUT].setChannels(_channels);
	outputs[EOC_OUTPUT].setVoltage(e.eocPulseGen.process(APP->engine->getSampleTime()) ? 5.0f : 0.0f, c);

	_attackLightSum += e.envelope.isStage(ADSR::ATTACK_STAGE);
	_releaseLightSum += e.envelope.isStage(ADSR::RELEASE_STAGE);
}

void ASR::postProcess(const ProcessArgs& args) {
	lights[ATTACK_LIGHT].value = _attackLightSum / (float)_channels;
	lights[RELEASE_LIGHT].value = _releaseLightSum / (float)_channels;
}

struct ASRWidget : ModuleWidget {
	static constexpr int hp = 3;

	ASRWidget(ASR* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ASR.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto attackParamPosition = Vec(8.0, 33.0);
		auto releaseParamPosition = Vec(8.0, 90.0);
		auto sustainParamPosition = Vec(18.0, 130.0);
		auto linearParamPosition = Vec(26.0, 150.0);

		auto triggerInputPosition = Vec(10.5, 165.0);
		auto attackInputPosition = Vec(10.5, 200.0);
		auto releaseInputPosition = Vec(10.5, 235.0);

		auto envOutputPosition = Vec(10.5, 273.0);
		auto eocOutputPosition = Vec(10.5, 308.0);

		auto attackLightPosition = Vec(20.8, 65.0);
		auto releaseLightPosition = Vec(20.8, 122.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob29>(attackParamPosition, module, ASR::ATTACK_PARAM));
		addParam(createParam<Knob29>(releaseParamPosition, module, ASR::RELEASE_PARAM));
		addParam(createParam<Knob16>(sustainParamPosition, module, ASR::SUSTAIN_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(linearParamPosition, module, ASR::LINEAR_PARAM));

		addInput(createInput<Port24>(triggerInputPosition, module, ASR::TRIGGER_INPUT));
		addInput(createInput<Port24>(attackInputPosition, module, ASR::ATTACK_INPUT));
		addInput(createInput<Port24>(releaseInputPosition, module, ASR::RELEASE_INPUT));

		addOutput(createOutput<Port24>(envOutputPosition, module, ASR::ENV_OUTPUT));
		addOutput(createOutput<Port24>(eocOutputPosition, module, ASR::EOC_OUTPUT));

		addChild(createLight<TinyLight<GreenLight>>(attackLightPosition, module, ASR::ATTACK_LIGHT));
		addChild(createLight<TinyLight<GreenLight>>(releaseLightPosition, module, ASR::RELEASE_LIGHT));
	}
};

Model* modelASR = createModel<ASR, ASRWidget>("Bogaudio-ASR", "ASR", "Utility attack/sustain/release envelope generator", "Envelope generator", "Polyphonic");
