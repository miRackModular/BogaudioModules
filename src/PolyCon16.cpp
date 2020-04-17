
#include "PolyCon16.hpp"

void PolyCon16::processAll(const ProcessArgs& args) {
	int cn = clamp(params[CHANNELS_PARAM].getValue(), 1.0f, 16.0f);
	if (inputs[CHANNELS_INPUT].isConnected()) {
		cn = inputs[CHANNELS_INPUT].getChannels();
	}

	outputs[OUT_OUTPUT].setChannels(cn);
	for (int c = 0; c < cn; ++c) {
		float out = clamp(params[CHANNEL1_PARAM + c].getValue(), -1.0f, 1.0f);
		out += _rangeOffset;
		out *= _rangeScale;
		outputs[OUT_OUTPUT].setVoltage(out, c);
		lights[CHANNEL1_LIGHT + c].value = 1.0f;
	}
	for (int c = cn; c < maxChannels; ++c) {
		lights[CHANNEL1_LIGHT + c].value = 0.0f;
	}
}

struct PolyCon16Widget : ModuleWidget {
	static constexpr int hp = 8;

	PolyCon16Widget(PolyCon16* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/PolyCon16.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto channel1ParamPosition = Vec(25.3, 30.2);
		auto channel2ParamPosition = Vec(25.2, 67.2);
		auto channel3ParamPosition = Vec(25.2, 104.2);
		auto channel4ParamPosition = Vec(25.2, 141.2);
		auto channel5ParamPosition = Vec(25.2, 178.2);
		auto channel6ParamPosition = Vec(25.2, 215.2);
		auto channel7ParamPosition = Vec(25.2, 252.2);
		auto channel8ParamPosition = Vec(25.2, 289.2);
		auto channel9ParamPosition = Vec(75.2, 30.2);
		auto channel10ParamPosition = Vec(75.2, 67.2);
		auto channel11ParamPosition = Vec(75.2, 104.2);
		auto channel12ParamPosition = Vec(75.2, 141.2);
		auto channel13ParamPosition = Vec(75.2, 178.2);
		auto channel14ParamPosition = Vec(75.2, 215.2);
		auto channel15ParamPosition = Vec(75.2, 252.2);
		auto channel16ParamPosition = Vec(75.2, 289.2);
		auto channelsParamPosition = Vec(23.0, 332.0);

		auto channelsInputPosition = Vec(55.5, 322.0);

		auto outOutputPosition = Vec(86.5, 322.0);

		auto channel1LightPosition = Vec(33.75, 52.0);
		auto channel2LightPosition = Vec(33.75, 89.0);
		auto channel3LightPosition = Vec(33.75, 126.0);
		auto channel4LightPosition = Vec(33.75, 163.0);
		auto channel5LightPosition = Vec(33.75, 200.0);
		auto channel6LightPosition = Vec(33.75, 237.0);
		auto channel7LightPosition = Vec(33.75, 274.0);
		auto channel8LightPosition = Vec(33.75, 311.0);
		auto channel9LightPosition = Vec(83.75, 52.0);
		auto channel10LightPosition = Vec(83.75, 89.0);
		auto channel11LightPosition = Vec(83.75, 126.0);
		auto channel12LightPosition = Vec(83.75, 163.0);
		auto channel13LightPosition = Vec(83.75, 200.0);
		auto channel14LightPosition = Vec(83.75, 237.0);
		auto channel15LightPosition = Vec(83.75, 274.0);
		auto channel16LightPosition = Vec(83.75, 311.0);
		// end generated by svg_widgets.rb

		{
			auto w = createParam<Knob16>(channelsParamPosition, module, PolyCon16::CHANNELS_PARAM);
			dynamic_cast<Knob*>(w)->snap = true;
			addParam(w);
		}
		addParam(createParam<Knob19>(channel1ParamPosition, module, PolyCon16::CHANNEL1_PARAM));
		addParam(createParam<Knob19>(channel2ParamPosition, module, PolyCon16::CHANNEL2_PARAM));
		addParam(createParam<Knob19>(channel3ParamPosition, module, PolyCon16::CHANNEL3_PARAM));
		addParam(createParam<Knob19>(channel4ParamPosition, module, PolyCon16::CHANNEL4_PARAM));
		addParam(createParam<Knob19>(channel5ParamPosition, module, PolyCon16::CHANNEL5_PARAM));
		addParam(createParam<Knob19>(channel6ParamPosition, module, PolyCon16::CHANNEL6_PARAM));
		addParam(createParam<Knob19>(channel7ParamPosition, module, PolyCon16::CHANNEL7_PARAM));
		addParam(createParam<Knob19>(channel8ParamPosition, module, PolyCon16::CHANNEL8_PARAM));
		addParam(createParam<Knob19>(channel9ParamPosition, module, PolyCon16::CHANNEL9_PARAM));
		addParam(createParam<Knob19>(channel10ParamPosition, module, PolyCon16::CHANNEL10_PARAM));
		addParam(createParam<Knob19>(channel11ParamPosition, module, PolyCon16::CHANNEL11_PARAM));
		addParam(createParam<Knob19>(channel12ParamPosition, module, PolyCon16::CHANNEL12_PARAM));
		addParam(createParam<Knob19>(channel13ParamPosition, module, PolyCon16::CHANNEL13_PARAM));
		addParam(createParam<Knob19>(channel14ParamPosition, module, PolyCon16::CHANNEL14_PARAM));
		addParam(createParam<Knob19>(channel15ParamPosition, module, PolyCon16::CHANNEL15_PARAM));
		addParam(createParam<Knob19>(channel16ParamPosition, module, PolyCon16::CHANNEL16_PARAM));

		addInput(createInput<Port24>(channelsInputPosition, module, PolyCon16::CHANNELS_INPUT));

		addOutput(createOutput<Port24>(outOutputPosition, module, PolyCon16::OUT_OUTPUT));

		addChild(createLight<TinyLight<GreenLight>>(channel1LightPosition, module, PolyCon16::CHANNEL1_LIGHT));
		addChild(createLight<TinyLight<GreenLight>>(channel2LightPosition, module, PolyCon16::CHANNEL2_LIGHT));
		addChild(createLight<TinyLight<GreenLight>>(channel3LightPosition, module, PolyCon16::CHANNEL3_LIGHT));
		addChild(createLight<TinyLight<GreenLight>>(channel4LightPosition, module, PolyCon16::CHANNEL4_LIGHT));
		addChild(createLight<TinyLight<GreenLight>>(channel5LightPosition, module, PolyCon16::CHANNEL5_LIGHT));
		addChild(createLight<TinyLight<GreenLight>>(channel6LightPosition, module, PolyCon16::CHANNEL6_LIGHT));
		addChild(createLight<TinyLight<GreenLight>>(channel7LightPosition, module, PolyCon16::CHANNEL7_LIGHT));
		addChild(createLight<TinyLight<GreenLight>>(channel8LightPosition, module, PolyCon16::CHANNEL8_LIGHT));
		addChild(createLight<TinyLight<GreenLight>>(channel9LightPosition, module, PolyCon16::CHANNEL9_LIGHT));
		addChild(createLight<TinyLight<GreenLight>>(channel10LightPosition, module, PolyCon16::CHANNEL10_LIGHT));
		addChild(createLight<TinyLight<GreenLight>>(channel11LightPosition, module, PolyCon16::CHANNEL11_LIGHT));
		addChild(createLight<TinyLight<GreenLight>>(channel12LightPosition, module, PolyCon16::CHANNEL12_LIGHT));
		addChild(createLight<TinyLight<GreenLight>>(channel13LightPosition, module, PolyCon16::CHANNEL13_LIGHT));
		addChild(createLight<TinyLight<GreenLight>>(channel14LightPosition, module, PolyCon16::CHANNEL14_LIGHT));
		addChild(createLight<TinyLight<GreenLight>>(channel15LightPosition, module, PolyCon16::CHANNEL15_LIGHT));
		addChild(createLight<TinyLight<GreenLight>>(channel16LightPosition, module, PolyCon16::CHANNEL16_LIGHT));
	}

	void appendContextMenu(Menu* menu) override {
		menu->addChild(new MenuLabel());
		OutputRangeOptionMenuItem::addOutputRangeOptionsToMenu(module, menu);
	}
};

// Note slug is "Bogaudio-PolyCon" for backwards compatibility.
Model* modelPolyCon16 = createModel<PolyCon16, PolyCon16Widget>("Bogaudio-PolyCon", "POLYCON16", "Polyphonic per-channel constant voltages", "Polyphonic");
