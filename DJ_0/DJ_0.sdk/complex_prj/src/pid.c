#include "main.h"

void pid_calc(PID *v)
{	
    // Compute the error
    v->Err = v->Ref - v->Fdb;
    
    // Compute the proportional output
    v->Up = v->Kp*v->Err;
    if(v->Up > v->Up_Max)
    	v->Up = v->Up_Max;
    else if(v->Up < -1*(v->Up_Max))
    	v->Up = -1*(v->Up_Max);
    else
    	v->Up = v->Up;

    // Compute the integral output
    v->Ui = v->Ui + v->Ki*v->Up + v->Kc*v->SatErr;
    if(v->Ui > v->Ui_Max)
    	v->Ui = v->Ui_Max;
    else if(v->Ui < -1*(v->Ui_Max))
    	v->Ui = -1*(v->Ui_Max);
    else
    	v->Ui = v->Ui;
    
    // Compute the derivative output
    v->Ud = v->Kd*(v->Up - v->Up1);
    if(v->Ud > v->Ud_Max)
    	v->Ud = v->Ud_Max;
    else if(v->Ud < -1*(v->Ud_Max))
    	v->Ud = -1*(v->Ud_Max);
    else
    	v->Ud = v->Ud;

    // Compute the pre-saturated output
    v->OutPreSat = v->Up + v->Ui + v->Ud;     
    
    // Saturate the output
    if (v->OutPreSat > v->OutMax)                   
      v->Out =  v->OutMax;
    else if (v->OutPreSat < v->OutMin)
      v->Out =  v->OutMin;  
    else
      v->Out = v->OutPreSat;                   
    
    // Compute the saturate difference
    v->SatErr = v->Out - v->OutPreSat;     
    
    // Update the previous proportional output 
    v->Up1 = v->Up; 
    
}
void pid_clear(PID *v)
{
		v->Ui = 0;
		v->Ud = 0;
}



