

int Termocuple_K(int mV, int Tcool);
int Termocuple_B(int mV, int Tcool);
int Calc_mV( int ADC, char Kg);
int Calc_mA( int ADC, char Kg);
int Calc_Ohm(int mV);
int RTD_Cu100(int Ohm);
int RTD_Pt100(int Ohm);

int Line(int mA, int P_4, int P_20);

extern PID_run pid_r[4];

int UpdatePID(PID * pid, PID_run * pid_run, int error);
void initPID();

void Q_air_gas();

extern FLR Flr;
void current_ramp();
void current_back_ramp();

void setAlatmDateTime();
unsigned int time_delta();
unsigned int time_pack();
void time_UNpack(int time, LPC_Rtc_DateTime_t *Dt);






