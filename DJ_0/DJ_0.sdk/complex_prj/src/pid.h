#ifndef __PID_H__
#define __PID_H__


typedef struct {  float  Ref;   			// Input: Reference input
				  float  Fdb;   			// Input: Feedback input
				  float  Err;				// Variable: Error
				  float  Kp;				// Parameter: Proportional gain
				  float  Up;				// Variable: Proportional output
				  float  Ui;				// Variable: Integral output
				  float  Ud;				// Variable: Derivative output
				  float  Up_Max;		    // Parameter: Maximum Up
				  float  Ui_Max;		    // Parameter: Maximum Ui
				  float  Ud_Max;		    // Parameter: Maximum Ud
				  float  OutPreSat; 		// Variable: Pre-saturated output
				  float  OutMax;		    // Parameter: Maximum output
				  float  OutMin;	    	// Parameter: Minimum output
				  float  Out;   			// Output: PID output
				  float  SatErr;			// Variable: Saturated difference
				  float  Ki;			    // Parameter: Integral gain
				  float  Kc;		     	// Parameter: Integral correction gain
				  float  Kd; 		        // Parameter: Derivative gain
				  float  Up1;		   	    // History: Previous proportional output
		 	 	  void  (*calc)();	  	// Pointer to calculation function
					void  (*clear)();
				 }PID;
/*-----------------------------------------------------------------------------
Default initalizer for the PID object.
-----------------------------------------------------------------------------*/                     
#define PID_DEFAULTS     { 0.0f, \
                           0.0f, \
                           0.0f, \
						   1.2f, \
                           0.0f, \
                           0.0f, \
                           0.0f, \
						   4900.0f, \
						   1000.0f, \
						   1500.0f, \
                           0.0f, \
                           5000.0f, \
                           -5000.0f, \
                           0.0f, \
                           0.0f, \
                           0.0f, \
                           0.0f, \
                           0.0f, \
                           0.0f, \
              			  &pid_calc,\
											&pid_clear}

/*------------------------------------------------------------------------------
Prototypes for the functions in PID.C
------------------------------------------------------------------------------*/
void pid_calc(PID *v);
void pid_clear(PID *v);

#endif // __PID_H__