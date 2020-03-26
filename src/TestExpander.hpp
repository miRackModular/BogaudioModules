#pragma once

#include "bogaudio.hpp"
#include "expanders.hpp"

extern Model* modelTestExpanderBase;
extern Model* modelTestExpanderExtension;

namespace bogaudio {

struct TestExpanderMessage : MessageBase {
	int channels = 0;
	float sample[BGModule::maxChannels] {};
};

struct TestExpanderExtension;

struct TestExpanderBase : ExpandableModule<TestExpanderMessage, TestExpanderExtension> {
	enum ParamsIds {
		NUM_PARAMS
	};

	enum InputsIds {
		IN_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		COM_LIGHT,
		NUM_LIGHTS
	};

	TestExpanderBase() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	}

	int channels() override;
	void processAll(const ProcessArgs& args) override;
	void processChannel(const ProcessArgs& args, int c) override;
};

struct TestExpanderExtension : ExpanderModule<TestExpanderMessage, TestExpanderBase> {
	enum ParamsIds {
		NUM_PARAMS
	};

	enum InputsIds {
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		COM_LIGHT,
		NUM_LIGHTS
	};

	TestExpanderExtension() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	}

	void processAll(const ProcessArgs& args) override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
