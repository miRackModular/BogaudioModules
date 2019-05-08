
#include "EightOne.hpp"

#define SELECT_ON_CLOCK "select_on_clock"

void EightOne::onReset() {
	_step = 0;
	_clock.reset();
	_reset.reset();
}

void EightOne::onSampleRateChange() {
	_timer.setParams(engineGetSampleRate(), 0.001f);
}

json_t* EightOne::toJson() {
	json_t* root = json_object();
	json_object_set_new(root, SELECT_ON_CLOCK, json_boolean(_selectOnClock));
	return root;
}

void EightOne::fromJson(json_t* root) {
	json_t* s = json_object_get(root, SELECT_ON_CLOCK);
	if (s) {
		_selectOnClock = json_is_true(s);
	}
}

void EightOne::step() {
	bool reset = _reset.process(inputs[RESET_INPUT].value);
	if (reset) {
		_timer.reset();
	}
	bool timer = _timer.next();
	bool clock = _clock.process(inputs[CLOCK_INPUT].value) && !timer;

	int steps = params[STEPS_PARAM].value;
	int reverse = 1 - 2 * (params[DIRECTION_PARAM].value == 0.0f);
	_step = (_step + reverse * clock) % steps;
	_step += (_step < 0) * steps;
	_step -= _step * reset;
	int select = params[SELECT_PARAM].value;
	select += clamp(inputs[SELECT_INPUT].value, 0.0f, 10.0f) * 0.1f * 8.0f;
	if (!_selectOnClock || clock) {
		_select = select;
	}
	int step = _step + _select;
	step = step % 8;

	float out = 0.0f;
	for (int i = 0; i < 8; ++i) {
		out += inputs[IN1_INPUT + i].value * (step == i);
		lights[IN1_LIGHT + i].value = step == i;
	}
	outputs[OUT_OUTPUT].value = out;
}

struct SelectOnClockMenuItem : MenuItem {
	EightOne* _module;

	SelectOnClockMenuItem(EightOne* module, const char* label)
	: _module(module)
	{
		this->text = label;
	}

	void onAction(EventAction &e) override {
		_module->_selectOnClock = !_module->_selectOnClock;
	}

	void step() override {
		rightText = _module->_selectOnClock ? "✔" : "";
	}
};

struct EightOneWidget : ModuleWidget {
	static constexpr int hp = 6;

	EightOneWidget(EightOne* module) : ModuleWidget(module) {
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SVGPanel *panel = new SVGPanel();
			panel->box.size = box.size;
			panel->setBackground(SVG::load(assetPlugin(plugin, "res/EightOne.svg")));
			addChild(panel);
		}

		addChild(Widget::create<ScrewSilver>(Vec(0, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto stepsParamPosition = Vec(58.5, 131.5);
		auto directionParamPosition = Vec(58.5, 167.5);
		auto selectParamPosition = Vec(52.0, 230.0);

		auto in1InputPosition = Vec(11.5, 35.0);
		auto in2InputPosition = Vec(11.5, 76.3);
		auto in3InputPosition = Vec(11.5, 118.6);
		auto in4InputPosition = Vec(11.5, 158.9);
		auto in5InputPosition = Vec(11.5, 200.1);
		auto in6InputPosition = Vec(11.5, 241.4);
		auto in7InputPosition = Vec(11.5, 282.7);
		auto in8InputPosition = Vec(11.5, 324.0);
		auto clockInputPosition = Vec(54.5, 35.0);
		auto resetInputPosition = Vec(54.5, 72.0);
		auto selectInputPosition = Vec(54.5, 270.0);

		auto outOutputPosition = Vec(54.5, 324.0);

		auto in1LightPosition = Vec(23.5, 61.5);
		auto in2LightPosition = Vec(23.5, 102.8);
		auto in3LightPosition = Vec(23.5, 145.1);
		auto in4LightPosition = Vec(23.5, 185.4);
		auto in5LightPosition = Vec(23.5, 226.6);
		auto in6LightPosition = Vec(23.5, 267.9);
		auto in7LightPosition = Vec(23.5, 309.2);
		auto in8LightPosition = Vec(23.5, 350.5);
		// end generated by svg_widgets.rb

		{
			auto w = ParamWidget::create<Knob16>(stepsParamPosition, module, EightOne::STEPS_PARAM, 1.0, 8.0, 8.0);
			dynamic_cast<Knob*>(w)->snap = true;
			addParam(w);
		}
		addParam(ParamWidget::create<SliderSwitch2State14>(directionParamPosition, module, EightOne::DIRECTION_PARAM, 0.0, 1.0, 1.0));
		{
			auto w = ParamWidget::create<Knob29>(selectParamPosition, module, EightOne::SELECT_PARAM, 0.0, 7.0, 0.0);
			dynamic_cast<Knob*>(w)->snap = true;
			addParam(w);
		}

		addInput(Port::create<Port24>(in1InputPosition, Port::INPUT, module, EightOne::IN1_INPUT));
		addInput(Port::create<Port24>(in2InputPosition, Port::INPUT, module, EightOne::IN2_INPUT));
		addInput(Port::create<Port24>(in3InputPosition, Port::INPUT, module, EightOne::IN3_INPUT));
		addInput(Port::create<Port24>(in4InputPosition, Port::INPUT, module, EightOne::IN4_INPUT));
		addInput(Port::create<Port24>(in5InputPosition, Port::INPUT, module, EightOne::IN5_INPUT));
		addInput(Port::create<Port24>(in6InputPosition, Port::INPUT, module, EightOne::IN6_INPUT));
		addInput(Port::create<Port24>(in7InputPosition, Port::INPUT, module, EightOne::IN7_INPUT));
		addInput(Port::create<Port24>(in8InputPosition, Port::INPUT, module, EightOne::IN8_INPUT));
		addInput(Port::create<Port24>(clockInputPosition, Port::INPUT, module, EightOne::CLOCK_INPUT));
		addInput(Port::create<Port24>(resetInputPosition, Port::INPUT, module, EightOne::RESET_INPUT));
		addInput(Port::create<Port24>(selectInputPosition, Port::INPUT, module, EightOne::SELECT_INPUT));

		addOutput(Port::create<Port24>(outOutputPosition, Port::OUTPUT, module, EightOne::OUT_OUTPUT));

		addChild(ModuleLightWidget::create<SmallLight<GreenLight>>(in1LightPosition, module, EightOne::IN1_LIGHT));
		addChild(ModuleLightWidget::create<SmallLight<GreenLight>>(in2LightPosition, module, EightOne::IN2_LIGHT));
		addChild(ModuleLightWidget::create<SmallLight<GreenLight>>(in3LightPosition, module, EightOne::IN3_LIGHT));
		addChild(ModuleLightWidget::create<SmallLight<GreenLight>>(in4LightPosition, module, EightOne::IN4_LIGHT));
		addChild(ModuleLightWidget::create<SmallLight<GreenLight>>(in5LightPosition, module, EightOne::IN5_LIGHT));
		addChild(ModuleLightWidget::create<SmallLight<GreenLight>>(in6LightPosition, module, EightOne::IN6_LIGHT));
		addChild(ModuleLightWidget::create<SmallLight<GreenLight>>(in7LightPosition, module, EightOne::IN7_LIGHT));
		addChild(ModuleLightWidget::create<SmallLight<GreenLight>>(in8LightPosition, module, EightOne::IN8_LIGHT));
	}

	void appendContextMenu(Menu* menu) override {
		EightOne* m = dynamic_cast<EightOne*>(module);
		assert(m);
		menu->addChild(new MenuLabel());
		menu->addChild(new SelectOnClockMenuItem(m, "Select on clock"));
	}
};

Model* modelEightOne = createModel<EightOne, EightOneWidget>("Bogaudio-EightOne", "8:1", "demux & sequential switch", SWITCH_TAG);