// Macros for declaring and defining various things that are needed by the
// standard group of controls that accompany a CSlider.
// Used by the subclasses of CDevAppPage that use sliders to control a DCAM
//  feature. Avoids typing lots of very repetetive code.

// Variable declarations (.h file, in class definition):
#define SLIDER_CONTROL_SUITE(name)				\
	CSlider			m_ ## name ## Slider;		\
	CFeatureEdit	m_ ## name ## Edit;			\
	CButton			m_ ## name ## AutoButton;	\
	CStatic			m_ ## name ## MinLabel;		\
	CStatic			m_ ## name ## MaxLabel;		\
	CStatic			m_ ## name ## Frame;		\

// Event handler declarations (.h file, in class definition)
#define SLIDER_HANDLER_SUITE(name)						\
	void Populate ## name ##(void);						\
	afx_msg void OnScroll ## name ## Slider(void);		\
	afx_msg void OnChange ## name ## Slider(void);		\
	afx_msg void OnKillFocus ## name ## Edit(void);		\
	afx_msg void OnClick ## name ## AutoButton(void);	\

// Very similar to SLIDER_HANDLER_SUITE, but the user supplied their own populate
#define SLIDER_EVENT_HANDLER_SUITE(name)				\
	afx_msg void OnScroll ## name ## Slider(void);		\
	afx_msg void OnChange ## name ## Slider(void);		\
	afx_msg void OnKillFocus ## name ## Edit(void);		\
	afx_msg void OnClick ## name ## AutoButton(void);	\

// DDX entries in DoDataExchange (.cpp file, inside DoDataExchange):
#define DDX_SLIDER_SUITE(ID,name)											\
	DDX_Control(pDX, IDC_ ## ID ## _SLIDER, m_ ## name ## Slider);			\
	DDX_Control(pDX, IDC_ ## ID ## _EDIT, m_ ## name ## Edit);				\
	DDX_Control(pDX, IDC_ ## ID ## AUTO_BUTTON, m_ ## name ## AutoButton);	\
	DDX_Control(pDX, IDC_ ## ID ## MIN_LABEL, m_ ## name ## MinLabel);		\
	DDX_Control(pDX, IDC_ ## ID ## MAX_LABEL, m_ ## name ## MaxLabel);		\
	DDX_Control(pDX, IDC_ ## ID ## _FRAME, m_ ## name ## Frame);			\

// Message Map entries (.cpp file, inside Message map):
#define MM_SLIDER_SUITE(ID,name)											\
	ON_EN_KILLFOCUS(IDC_ ## ID ## _EDIT, OnKillFocus ## name ## Edit)		\
	ON_BN_CLICKED(IDC_ ## ID ## AUTO_BUTTON, OnClick ## name ## AutoButton)	\

// EventSink Map entries (.cpp file, inside EventSink map):
#define EVENTSINK_SLIDER_SUITE(cls,ID,name)											\
	ON_EVENT(cls, IDC_ ## ID ## _SLIDER, 1, OnScroll ## name ## Slider, VTS_NONE)	\
	ON_EVENT(cls, IDC_ ## ID ## _SLIDER, 2, OnChange ## name ## Slider, VTS_NONE)	\

// Enable / Disable according to whether DCAM feature is supported (.cpp file, inside SetGuiState)
#define ENABLE_SLIDER_SUITE(name,feature)																\
	m_ ## name ## Slider.put_Enabled(enable && cam->FeatureSupportsManual(FEATURE_ ## feature));		\
	m_ ## name ## Edit.EnableWindow(enable && cam->FeatureSupported(FEATURE_ ## feature));				\
	if (enable && cam->FeatureSupported(FEATURE_ ## feature))											\
		m_ ## name ## Edit.SetReadOnly(!cam->FeatureSupportsManual(FEATURE_ ## feature));				\
	m_ ## name ## AutoButton.EnableWindow(enable && cam->FeatureSupportsOnePush(FEATURE_ ## feature));	\

// Implementation of various CSlider-related functions (.cpp file):
#define SLIDER_CONTROL_HANDLER_IMPL(CLASS,NAME,FEATURE)								\
	void CLASS::Populate ## NAME (void) {											\
		PopulateSlider(m_ ## NAME ##Slider, FEATURE_ ## FEATURE);					\
	}																				\
	void CLASS::OnScroll ## NAME ## Slider() {										\
		SliderScrolled(m_ ## NAME ## Slider, FEATURE_ ## FEATURE);					\
	}																				\
	void CLASS::OnChange ## NAME ## Slider() {										\
		SliderChanged(m_ ## NAME ## Slider, FEATURE_ ## FEATURE);					\
	}																				\
	void CLASS::OnKillFocus ## NAME ## Edit() {										\
		SliderEditFocusKilled(m_ ## NAME ## Slider, FEATURE_ ## FEATURE);			\
	}																				\
	void CLASS::OnClick ## NAME ## AutoButton() {									\
		/*CWaitCursor wc;*/															\
		try { GetActiveCamera()->SetOnePush(FEATURE_ ## FEATURE); }					\
		catch (PxLException const& e) { OnApiError(e); }							\
	}																				\

// Very similar to SLIDER_CONTROL_HANDLER_IMPL, but the user supplied thier own populate
#define SLIDER_CONTROL_EVENT_HANDLER_IMPL(CLASS,NAME,FEATURE)						\
	void CLASS::OnScroll ## NAME ## Slider() {										\
		SliderScrolled(m_ ## NAME ## Slider, FEATURE_ ## FEATURE);					\
	}																				\
	void CLASS::OnChange ## NAME ## Slider() {										\
		SliderChanged(m_ ## NAME ## Slider, FEATURE_ ## FEATURE);					\
	}																				\
	void CLASS::OnKillFocus ## NAME ## Edit() {										\
		SliderEditFocusKilled(m_ ## NAME ## Slider, FEATURE_ ## FEATURE);			\
	}																				\
	void CLASS::OnClick ## NAME ## AutoButton() {									\
		/*CWaitCursor wc;*/															\
		try { GetActiveCamera()->SetOnePush(FEATURE_ ## FEATURE); }					\
		catch (PxLException const& e) { OnApiError(e); }							\
	}																				\


