#include "rack.hpp"

using namespace rack;

extern Plugin *plugin;

namespace bogaudio {

struct Button18 : SVGSwitch, MomentarySwitch {
	Button18();
};

struct BGKnob : RoundKnob {
	BGKnob(const char* svg, int dim);
};

struct Knob16 : BGKnob {
	Knob16();
};

struct Knob26 : BGKnob {
	Knob26();
};

struct Knob29 : BGKnob {
	Knob29();
};

struct Knob38 : BGKnob {
	Knob38();
};

struct Knob68 : BGKnob {
	Knob68();
};

struct Port24 : SVGPort {
	Port24();
};

struct SliderSwitch : SVGSwitch, ToggleSwitch {
	CircularShadow* shadow = NULL;
	SliderSwitch();
};

struct SliderSwitch2State14 : SliderSwitch {
	SliderSwitch2State14();
};

struct StatefulButton : ParamWidget, FramebufferWidget {
	std::vector<std::shared_ptr<SVG>> _frames;
	SVGWidget* _svgWidget; // deleted elsewhere.
	CircularShadow* shadow = NULL;

	StatefulButton(const char* offSVGPath, const char* onSVGPath);
	void step() override;
	void onDragStart(EventDragStart& e) override;
	void onDragEnd(EventDragEnd& e) override;
};

struct StatefulButton9 : StatefulButton {
	StatefulButton9();
};

} // namespace bogaudio
