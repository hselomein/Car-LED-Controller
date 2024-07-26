class vehicle {
public:
// Startup Configuration (Constants)
    float curDRL   = 0.0f;
    float curHorn  = 0.0f;
    float curInd_L = 0.0f;
    float curInd_R = 0.0f;
    String drlState;
    String hornState;
    bool uberDisp;
    bool lyftDisp;
    Button* Horn_Button = NULL;
    Button* Ind_L_Button = NULL;
    Button* Ind_R_Button = NULL;
    bool isHazardRunning = false; // Global variable to track hazard function state

#if LED_STRIP
    enum indState {
	OFF = 0,
    RIGHT,
	LEFT,
	HAZARD
	};

    enum indState indStatus = OFF;
    indState currentIndicatorState = OFF; // Initialize with OFF or any default state
#endif
//private:
};