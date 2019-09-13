
#include "UMix.hpp"

#define SUM "sum"
#define CVMODE "cv_mode"

json_t* UMix::dataToJson() {
	json_t* root = json_object();
	json_object_set_new(root, SUM, json_boolean(_sum));
	json_object_set_new(root, CVMODE, json_boolean(_cvMode));
	return root;
}

void UMix::dataFromJson(json_t* root) {
	json_t* s = json_object_get(root, SUM);
	if (s) {
		_sum = json_is_true(s);
	}

	json_t* c = json_object_get(root, CVMODE);
	if (c) {
		_cvMode = json_is_true(c);
	}
}

bool UMix::active() {
	return outputs[OUT_OUTPUT].isConnected();
}

int UMix::channels() {
	int max = 0;
	for (int i = 0; i < 8; ++i) {
		max = std::max(max, inputs[IN1_INPUT + i].getChannels());
	}
	return max;
}

void UMix::processChannel(const ProcessArgs& args, int c) {
	outputs[OUT_OUTPUT].setChannels(_channels);

	if (_sum) {
		float out = 0.0f;
		for (int i = 0; i < 8; ++i) {
			out += inputs[IN1_INPUT + i].getPolyVoltage(c);
		}
		out *= params[LEVEL_PARAM].getValue();
		if (_cvMode) {
			outputs[OUT_OUTPUT].setVoltage(clamp(out, -12.0f, 12.0f), c);
		}
		else {
			outputs[OUT_OUTPUT].setVoltage(_saturator[c].next(out), c);
		}
	}
	else {
		float out = 0.0f;
		int active = 0;
		for (int i = 0; i < 8; ++i) {
			if (inputs[IN1_INPUT + i].isConnected()) {
				out += inputs[IN1_INPUT + i].getPolyVoltage(c);
				++active;
			}
		}
		if (active > 0) {
			out /= (float)active;
			out *= params[LEVEL_PARAM].getValue();
			if (_cvMode) {
				outputs[OUT_OUTPUT].setVoltage(clamp(out, -12.0f, 12.0f), c);
			}
			else {
				outputs[OUT_OUTPUT].setVoltage(_saturator[c].next(out), c);
			}
		}
		else {
			outputs[OUT_OUTPUT].setVoltage(0.0f, c);
		}
	}
}

struct UMixWidget : ModuleWidget {
	static constexpr int hp = 3;

	UMixWidget(UMix* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/UMix.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto levelParamPosition = Vec(14.5, 320.5);

		auto in1InputPosition = Vec(10.5, 23.0);
		auto in2InputPosition = Vec(10.5, 53.0);
		auto in3InputPosition = Vec(10.5, 83.0);
		auto in4InputPosition = Vec(10.5, 113.0);
		auto in5InputPosition = Vec(10.5, 143.0);
		auto in6InputPosition = Vec(10.5, 173.0);
		auto in7InputPosition = Vec(10.5, 203.0);
		auto in8InputPosition = Vec(10.5, 233.0);

		auto outOutputPosition = Vec(10.5, 263.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob16>(levelParamPosition, module, UMix::LEVEL_PARAM));

		addInput(createInput<Port24>(in1InputPosition, module, UMix::IN1_INPUT));
		addInput(createInput<Port24>(in2InputPosition, module, UMix::IN2_INPUT));
		addInput(createInput<Port24>(in3InputPosition, module, UMix::IN3_INPUT));
		addInput(createInput<Port24>(in4InputPosition, module, UMix::IN4_INPUT));
		addInput(createInput<Port24>(in5InputPosition, module, UMix::IN5_INPUT));
		addInput(createInput<Port24>(in6InputPosition, module, UMix::IN6_INPUT));
		addInput(createInput<Port24>(in7InputPosition, module, UMix::IN7_INPUT));
		addInput(createInput<Port24>(in8InputPosition, module, UMix::IN8_INPUT));

		addOutput(createOutput<Port24>(outOutputPosition, module, UMix::OUT_OUTPUT));
	}

	struct AverageMenuItem : MenuItem {
		UMix* _module;

		AverageMenuItem(UMix* module, const char* label)
		: _module(module)
		{
			this->text = label;
		}

		void onAction(const event::Action& e) override {
			_module->_sum = !_module->_sum;
		}

		void step() override {
			rightText = !_module->_sum ? "✔" : "";
		}
	};

	struct CVModeMenuItem : MenuItem {
		UMix* _module;

		CVModeMenuItem(UMix* module, const char* label)
		: _module(module)
		{
			this->text = label;
		}

		void onAction(const event::Action& e) override {
			_module->_cvMode = !_module->_cvMode;
		}

		void step() override {
			rightText = _module->_cvMode ? "✔" : "";
		}
	};

	void appendContextMenu(Menu* menu) override {
		UMix* umix = dynamic_cast<UMix*>(module);
		assert(umix);
		menu->addChild(new MenuLabel());
		menu->addChild(new AverageMenuItem(umix, "Average"));
		menu->addChild(new CVModeMenuItem(umix, "CV mode"));
	}
};

Model* modelUMix = bogaudio::createModel<UMix, UMixWidget>("Bogaudio-UMix", "UMix", "unity mixer", "Mixer");
