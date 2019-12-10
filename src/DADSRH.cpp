
#include "DADSRH.hpp"

void DADSRH::reset() {
	for (int c = 0; c < _channels; ++c) {
		_core[c]->reset();
	}
}

int DADSRH::channels() {
	return inputs[TRIGGER_INPUT].getChannels();
}

void DADSRH::addChannel(int c) {
	_core[c] = new DADSRHCore(
		params[DELAY_PARAM],
		params[ATTACK_PARAM],
		params[DECAY_PARAM],
		params[SUSTAIN_PARAM],
		params[RELEASE_PARAM],
		params[HOLD_PARAM],
		params[ATTACK_SHAPE_PARAM],
		params[DECAY_SHAPE_PARAM],
		params[RELEASE_SHAPE_PARAM],
		params[TRIGGER_PARAM],
		params[MODE_PARAM],
		params[LOOP_PARAM],
		params[SPEED_PARAM],
		params[RETRIGGER_PARAM],

		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		inputs[TRIGGER_INPUT],

		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		outputs[ENV_OUTPUT],
		outputs[INV_OUTPUT],
		outputs[TRIGGER_OUTPUT],

		_delayLights,
		_attackLights,
		_decayLights,
		_sustainLights,
		_releaseLights,
		lights[ATTACK_SHAPE1_LIGHT],
		lights[ATTACK_SHAPE2_LIGHT],
		lights[ATTACK_SHAPE3_LIGHT],
		lights[DECAY_SHAPE1_LIGHT],
		lights[DECAY_SHAPE2_LIGHT],
		lights[DECAY_SHAPE3_LIGHT],
		lights[RELEASE_SHAPE1_LIGHT],
		lights[RELEASE_SHAPE2_LIGHT],
		lights[RELEASE_SHAPE3_LIGHT],

		_triggerOnLoad,
		_shouldTriggerOnLoad
	);
}

void DADSRH::removeChannel(int c) {
	delete _core[c];
	_core[c] = NULL;
}

void DADSRH::processChannel(const ProcessArgs& args, int c) {
	_core[c]->step(c, _channels);
}

void DADSRH::postProcess(const ProcessArgs& args) {
	float delaySum = 0.0f;
	float attackSum = 0.0f;
	float decaySum = 0.0f;
	float sustainSum = 0.0f;
	float releaseSum = 0.0f;
	for (int c = 0; c < _channels; ++c) {
		delaySum += _delayLights[c];
		attackSum += _attackLights[c];
		decaySum += _decayLights[c];
		sustainSum += _sustainLights[c];
		releaseSum += _releaseLights[c];
	}
	lights[DELAY_LIGHT].value = delaySum / (float)_channels;
	lights[ATTACK_LIGHT].value = attackSum / (float)_channels;
	lights[DECAY_LIGHT].value = decaySum / (float)_channels;
	lights[SUSTAIN_LIGHT].value = sustainSum / (float)_channels;
	lights[RELEASE_LIGHT].value = releaseSum / (float)_channels;
}

bool DADSRH::shouldTriggerOnNextLoad() {
	for (int c = 0; c < _channels; ++c) {
		if (_core[c] && _core[c]->_stage != _core[c]->STOPPED_STAGE) {
			return true;
		}
	}
	return false;
}

struct DADSRHWidget : TriggerOnLoadModuleWidget {
	static constexpr int hp = 10;

	DADSRHWidget(DADSRH* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/DADSRH.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 0)));
		addChild(createWidget<ScrewSilver>(Vec(0, 365)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto delayParamPosition = Vec(27.08, 33.08);
		auto attackParamPosition = Vec(27.08, 89.08);
		auto decayParamPosition = Vec(27.08, 145.08);
		auto sustainParamPosition = Vec(27.08, 201.08);
		auto releaseParamPosition = Vec(27.08, 257.08);
		auto holdParamPosition = Vec(82.38, 313.08);
		auto attackShapeParamPosition = Vec(77.02, 124.02);
		auto decayShapeParamPosition = Vec(77.02, 180.52);
		auto releaseShapeParamPosition = Vec(77.02, 292.52);
		auto triggerParamPosition = Vec(90.04, 43.04);
		auto modeParamPosition = Vec(120.0, 95.0);
		auto loopParamPosition = Vec(120.0, 145.0);
		auto speedParamPosition = Vec(20.0, 321.0);
		auto retriggerParamPosition = Vec(55.0, 321.0);

		auto triggerInputPosition = Vec(115.0, 40.0);

		auto envOutputPosition = Vec(115.0, 191.0);
		auto invOutputPosition = Vec(115.0, 228.0);
		auto triggerOutputPosition = Vec(115.0, 265.0);

		auto delayLightPosition = Vec(12.0, 76.0);
		auto attackLightPosition = Vec(12.0, 123.0);
		auto decayLightPosition = Vec(12.0, 179.0);
		auto sustainLightPosition = Vec(12.0, 235.0);
		auto releaseLightPosition = Vec(12.0, 291.0);
		auto attackShape1LightPosition = Vec(77.0, 96.0);
		auto attackShape2LightPosition = Vec(77.0, 106.0);
		auto attackShape3LightPosition = Vec(77.0, 116.0);
		auto decayShape1LightPosition = Vec(77.0, 152.5);
		auto decayShape2LightPosition = Vec(77.0, 162.5);
		auto decayShape3LightPosition = Vec(77.0, 172.5);
		auto releaseShape1LightPosition = Vec(77.0, 264.5);
		auto releaseShape2LightPosition = Vec(77.0, 274.5);
		auto releaseShape3LightPosition = Vec(77.0, 284.5);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob38>(delayParamPosition, module, DADSRH::DELAY_PARAM));
		addParam(createParam<Knob38>(attackParamPosition, module, DADSRH::ATTACK_PARAM));
		addParam(createParam<Knob38>(decayParamPosition, module, DADSRH::DECAY_PARAM));
		addParam(createParam<Knob38>(sustainParamPosition, module, DADSRH::SUSTAIN_PARAM));
		addParam(createParam<Knob38>(releaseParamPosition, module, DADSRH::RELEASE_PARAM));
		addParam(createParam<Knob38>(holdParamPosition, module, DADSRH::HOLD_PARAM));
		addParam(createParam<StatefulButton9>(attackShapeParamPosition, module, DADSRH::ATTACK_SHAPE_PARAM));
		addParam(createParam<StatefulButton9>(decayShapeParamPosition, module, DADSRH::DECAY_SHAPE_PARAM));
		addParam(createParam<StatefulButton9>(releaseShapeParamPosition, module, DADSRH::RELEASE_SHAPE_PARAM));
		addParam(createParam<Button18>(triggerParamPosition, module, DADSRH::TRIGGER_PARAM));
		addParam(createParam<SliderSwitch2State14>(modeParamPosition, module, DADSRH::MODE_PARAM));
		addParam(createParam<SliderSwitch2State14>(loopParamPosition, module, DADSRH::LOOP_PARAM));
		addParam(createParam<SliderSwitch2State14>(speedParamPosition, module, DADSRH::SPEED_PARAM));
		addParam(createParam<SliderSwitch2State14>(retriggerParamPosition, module, DADSRH::RETRIGGER_PARAM));

		addInput(createInput<Port24>(triggerInputPosition, module, DADSRH::TRIGGER_INPUT));

		addOutput(createOutput<Port24>(envOutputPosition, module, DADSRH::ENV_OUTPUT));
		addOutput(createOutput<Port24>(invOutputPosition, module, DADSRH::INV_OUTPUT));
		addOutput(createOutput<Port24>(triggerOutputPosition, module, DADSRH::TRIGGER_OUTPUT));

		addChild(createLight<TinyLight<GreenLight>>(delayLightPosition, module, DADSRH::DELAY_LIGHT));
		addChild(createLight<TinyLight<GreenLight>>(attackLightPosition, module, DADSRH::ATTACK_LIGHT));
		addChild(createLight<TinyLight<GreenLight>>(decayLightPosition, module, DADSRH::DECAY_LIGHT));
		addChild(createLight<TinyLight<GreenLight>>(sustainLightPosition, module, DADSRH::SUSTAIN_LIGHT));
		addChild(createLight<TinyLight<GreenLight>>(releaseLightPosition, module, DADSRH::RELEASE_LIGHT));
		addChild(createLight<TinyLight<GreenLight>>(attackShape1LightPosition, module, DADSRH::ATTACK_SHAPE1_LIGHT));
		addChild(createLight<TinyLight<GreenLight>>(attackShape2LightPosition, module, DADSRH::ATTACK_SHAPE2_LIGHT));
		addChild(createLight<TinyLight<GreenLight>>(attackShape3LightPosition, module, DADSRH::ATTACK_SHAPE3_LIGHT));
		addChild(createLight<TinyLight<GreenLight>>(decayShape1LightPosition, module, DADSRH::DECAY_SHAPE1_LIGHT));
		addChild(createLight<TinyLight<GreenLight>>(decayShape2LightPosition, module, DADSRH::DECAY_SHAPE2_LIGHT));
		addChild(createLight<TinyLight<GreenLight>>(decayShape3LightPosition, module, DADSRH::DECAY_SHAPE3_LIGHT));
		addChild(createLight<TinyLight<GreenLight>>(releaseShape1LightPosition, module, DADSRH::RELEASE_SHAPE1_LIGHT));
		addChild(createLight<TinyLight<GreenLight>>(releaseShape2LightPosition, module, DADSRH::RELEASE_SHAPE2_LIGHT));
		addChild(createLight<TinyLight<GreenLight>>(releaseShape3LightPosition, module, DADSRH::RELEASE_SHAPE3_LIGHT));
	}
};

Model* modelDADSRH = bogaudio::createModel<DADSRH, DADSRHWidget>("Bogaudio-DADSRH", "DADSR(H)", "Advanced envelope generator", "Envelope generator", "Polyphonic");
