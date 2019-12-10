
#include "Walk.hpp"

#define POLY_INPUT "poly_input"

void Walk::reset() {
	for (int i = 0; i < maxChannels; ++i) {
		_jumpTrigger[i].reset();
	}
}

void Walk::sampleRateChange() {
	for (int i = 0; i < maxChannels; ++i) {
		_slew[i].setParams(APP->engine->getSampleRate(), 100.0f, 10.0f);
	}
}

json_t* Walk::dataToJson() {
	json_t* root = json_object();
	json_object_set_new(root, POLY_INPUT, json_integer(_polyInputID));
	return root;
}

void Walk::dataFromJson(json_t* root) {
	json_t* p = json_object_get(root, POLY_INPUT);
	if (p) {
		_polyInputID = json_integer_value(p);
	}
}

int Walk::channels() {
	int id = _polyInputID;
	if (!(_polyInputID == OFFSET_INPUT || _polyInputID == SCALE_INPUT || _polyInputID == JUMP_INPUT)) {
		id = RATE_INPUT;
	}
	return inputs[id].getChannels();
}

void Walk::modulateChannel(int c) {
	float rate = params[RATE_PARAM].getValue();
	if (inputs[RATE_INPUT].isConnected()) {
		rate *= clamp(inputs[RATE_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}
	rate = 0.2f * powf(rate, 5.0f);
	_walk[c].setParams(APP->engine->getSampleRate(), rate);

	_offset[c] = params[OFFSET_PARAM].getValue();
	if (inputs[OFFSET_INPUT].isConnected()) {
		_offset[c] *= clamp(inputs[OFFSET_INPUT].getPolyVoltage(c) / 5.0f, -1.0f, 1.0f);
	}
	_offset[c] *= 5.0f;

	_scale[c] = params[SCALE_PARAM].getValue();
	if (inputs[SCALE_INPUT].isConnected()) {
		_scale[c] *= clamp(inputs[SCALE_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}
}

void Walk::processChannel(const ProcessArgs& args, int c) {
	if (_jumpTrigger[c].process(inputs[JUMP_INPUT].getPolyVoltage(c))) {
		_walk[c].jump();
	}

	float out = _slew[c].next(_walk[c].next());
	out *= _scale[c];
	out += _offset[c];
	outputs[OUT_OUTPUT].setChannels(_channels);
	outputs[OUT_OUTPUT].setVoltage(out, c);
}

struct WalkWidget : ModuleWidget {
	static constexpr int hp = 3;

	WalkWidget(Walk* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Walk.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto rateParamPosition = Vec(8.0, 36.0);
		auto offsetParamPosition = Vec(14.5, 91.0);
		auto scaleParamPosition = Vec(14.5, 130.0);

		auto rateInputPosition = Vec(10.5, 160.0);
		auto offsetInputPosition = Vec(10.5, 195.0);
		auto scaleInputPosition = Vec(10.5, 230.0);
		auto jumpInputPosition = Vec(10.5, 265.0);

		auto outOutputPosition = Vec(10.5, 303.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob29>(rateParamPosition, module, Walk::RATE_PARAM));
		addParam(createParam<Knob16>(offsetParamPosition, module, Walk::OFFSET_PARAM));
		addParam(createParam<Knob16>(scaleParamPosition, module, Walk::SCALE_PARAM));

		addInput(createInput<Port24>(rateInputPosition, module, Walk::RATE_INPUT));
		addInput(createInput<Port24>(offsetInputPosition, module, Walk::OFFSET_INPUT));
		addInput(createInput<Port24>(scaleInputPosition, module, Walk::SCALE_INPUT));
		addInput(createInput<Port24>(jumpInputPosition, module, Walk::JUMP_INPUT));

		addOutput(createOutput<Port24>(outOutputPosition, module, Walk::OUT_OUTPUT));
	}

	void appendContextMenu(Menu* menu) override {
		Walk* m = dynamic_cast<Walk*>(module);
		assert(m);
		menu->addChild(new MenuLabel());
		OptionsMenuItem* p = new OptionsMenuItem("Polyphony channels from");
		p->addItem(OptionMenuItem("RATE input", [m]() { return m->_polyInputID == Walk::RATE_INPUT; }, [m]() { m->_polyInputID = Walk::RATE_INPUT; }));
		p->addItem(OptionMenuItem("OFFSET input", [m]() { return m->_polyInputID == Walk::OFFSET_INPUT; }, [m]() { m->_polyInputID = Walk::OFFSET_INPUT; }));
		p->addItem(OptionMenuItem("SCALE input", [m]() { return m->_polyInputID == Walk::SCALE_INPUT; }, [m]() { m->_polyInputID = Walk::SCALE_INPUT; }));
		p->addItem(OptionMenuItem("JUMP input", [m]() { return m->_polyInputID == Walk::JUMP_INPUT; }, [m]() { m->_polyInputID = Walk::JUMP_INPUT; }));
		OptionsMenuItem::addToMenu(p, menu);
	}
};

Model* modelWalk = bogaudio::createModel<Walk, WalkWidget>("Bogaudio-Walk", "WALK", "Random-walk CV source", "Random", "Polyphonic");
