/*
 * This file is part of Cleanflight and Betaflight.
 *
 * Cleanflight and Betaflight are free software. You can redistribute
 * this software and/or modify this software under the terms of the
 * GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Cleanflight and Betaflight are distributed in the hope that they
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <stdbool.h>
#include "common/time.h"
#include "common/filter.h"
#include "common/axis.h"
#include "pg/pg.h"

#define MAX_PID_PROCESS_DENOM       16
#define PID_CONTROLLER_BETAFLIGHT   1

#define PIDSUM_LIMIT                500
#define PIDSUM_LIMIT_YAW            500
#define PIDSUM_LIMIT_MIN            100
#define PIDSUM_LIMIT_MAX            1000

#define ROLL_PTERM_SCALE            0.0032029f
#define ROLL_ITERM_SCALE            0.0488762f
#define ROLL_DTERM_SCALE            0.0000529f

#define PITCH_PTERM_SCALE           0.0032029f
#define PITCH_ITERM_SCALE           0.0488762f
#define PITCH_DTERM_SCALE           0.0000529f

#define YAW_PTERM_SCALE             0.032029f
#define YAW_ITERM_SCALE             0.244381f
#define YAW_DTERM_SCALE             0.000529f

#define ROLL_FF_SCALE               0.00013754f
#define PITCH_FF_SCALE              0.00013754f
#define YAW_FF_SCALE                0.00013754f

// Full iterm suppression in setpoint mode at high-passed setpoint rate > 40deg/sec
#define ITERM_RELAX_SETPOINT_THRESHOLD 40.0f
#define ITERM_RELAX_CUTOFF_DEFAULT 15

#define ITERM_ACCELERATOR_GAIN_OFF 1000
#define ITERM_ACCELERATOR_GAIN_MAX 30000

typedef enum {
    PID_ROLL,
    PID_PITCH,
    PID_YAW,
    PID_LEVEL,
    PID_MAG,
    PID_ITEM_COUNT
} pidIndex_e;

typedef enum {
    SUPEREXPO_YAW_OFF = 0,
    SUPEREXPO_YAW_ON,
    SUPEREXPO_YAW_ALWAYS
} pidSuperExpoYaw_e;

typedef struct pidf_s {
    uint8_t P;
    uint8_t I;
    uint8_t D;
    uint16_t F;
} pidf_t;

typedef enum {
    ITERM_RELAX_OFF,
    ITERM_RELAX_RP,
    ITERM_RELAX_RPY,
    ITERM_RELAX_RP_INC,
    ITERM_RELAX_RPY_INC,
    ITERM_RELAX_COUNT,
} itermRelax_e;

typedef enum {
    ITERM_RELAX_GYRO,
    ITERM_RELAX_SETPOINT,
    ITERM_RELAX_TYPE_COUNT,
} itermRelaxType_e;

#define MAX_PROFILE_NAME_LENGTH 8u

typedef struct pidProfile_s {
    uint16_t yaw_lowpass_hz;                // Additional yaw filter when yaw axis too noisy
    uint16_t dterm_lowpass_hz;              // Delta Filter in hz
    uint16_t dterm_notch_hz;                // Biquad dterm notch hz
    uint16_t dterm_notch_cutoff;            // Biquad dterm notch low cutoff

    pidf_t  pid[PID_ITEM_COUNT];

    uint8_t dterm_filter_type;              // Filter selection for dterm
    uint8_t itermWindupPointPercent;        // iterm windup threshold, percent motor saturation
    uint16_t pidSumLimit;
    uint16_t pidSumLimitYaw;
    uint8_t levelAngleLimit;                // Max angle in degrees in level mode

    uint8_t horizon_tilt_effect;            // inclination factor for Horizon mode
    uint8_t horizon_tilt_expert_mode;       // OFF or ON

    // Betaflight PID controller parameters
    uint16_t yawRateAccelLimit;             // yaw accel limiter for deg/sec/ms
    uint16_t rateAccelLimit;                // accel limiter roll/pitch deg/sec/ms
    uint8_t feedForwardTransition;          // Feed forward weight transition
    uint16_t itermLimit;
    uint16_t dterm_lowpass2_hz;             // Extra PT1 Filter on D in hz
    uint8_t iterm_rotation;                 // rotates iterm to translate world errors to local coordinate system
    uint8_t iterm_relax_type;               // Specifies type of relax algorithm
    uint8_t iterm_relax_cutoff;             // This cutoff frequency specifies a low pass filter which predicts average response of the quad to setpoint
    uint8_t iterm_relax;                    // Enable iterm suppression during stick input
    uint8_t acro_trainer_angle_limit;       // Acro trainer roll/pitch angle limit in degrees
    uint8_t acro_trainer_debug_axis;        // The axis for which record debugging values are captured 0=roll, 1=pitch
    uint8_t acro_trainer_gain;              // The strength of the limiting. Raising may reduce overshoot but also lead to oscillation around the angle limit
    uint16_t acro_trainer_lookahead_ms;     // The lookahead window in milliseconds used to reduce overshoot
    uint8_t abs_control_gain;               // How strongly should the absolute accumulated error be corrected for
    uint8_t abs_control_limit;              // Limit to the correction
    uint8_t abs_control_error_limit;        // Limit to the accumulated error
    uint8_t abs_control_cutoff;             // Cutoff frequency for path estimation in abs control
    uint8_t dterm_filter2_type;             // Filter selection for 2nd dterm
    uint16_t dyn_lpf_dterm_min_hz;
    uint16_t dyn_lpf_dterm_max_hz;
    uint8_t thrustLinearization;            // Compensation factor for pid linearization
    uint8_t ff_boost;                       // amount of high-pass filtered FF to add to FF, 100 means 100% added
    char profileName[MAX_PROFILE_NAME_LENGTH + 1]; // Descriptive name for profile

    uint8_t ff_interpolate_sp;              // Calculate FF from interpolated setpoint
    uint8_t ff_max_rate_limit;              // Maximum setpoint rate percentage for FF
    uint8_t ff_spike_limit;                 // FF stick extrapolation lookahead period in ms
    uint8_t ff_smooth_factor;               // Amount of smoothing for interpolated FF steps
    uint8_t dyn_lpf_curve_expo;             // set the curve for dynamic dterm lowpass filter
    uint8_t vbat_sag_compensation;          // Reduce motor output by this percentage of the maximum compensation amount

    // HF3D parameters
    uint16_t yawColKf;                      // Feedforward for collective into Yaw
    uint16_t yawColPulseKf;                 // Feedforward for collective impulse into Yaw
    uint16_t yawCycKf;                      // Feedforward for cyclic into Yaw
    uint16_t yawBaseThrust;                 // Base thrust for the tail
    uint16_t collective_ff_impulse_freq;    // Collective input impulse high-pass filter cutoff frequency
    uint8_t error_decay_always;             // Always decay accumulated I term and Abs Control error?
    uint8_t error_decay_rate;               // Rate to decay accumulated error in deg/s
    uint8_t rescue_collective;             	// Collective pitch command when rescue is fully upright (100 = 8 degrees of pitch)
	uint8_t rescue_collective_boost;        // Collective pitch boost until rescue_delay has expired
    uint8_t rescue_delay;             		// T/10 before rolling non inverted :if==0, heli will immediately go to upright, 
											//30==3s inverted rescue before roll to upright: 35==disabe upright. Will just continue.  																												 
    uint16_t elevator_filter_gain;          // Elevator stop de-bounce feedforward filter gain
    uint8_t elevator_filter_window_time;    // Time in ms that we no longer apply de-bounce inside our window
    uint8_t elevator_filter_window_size;    // Size of the de-bounce window around center stick (0 deg/s) in degrees/second
    uint8_t elevator_filter_hz;             // Low-pass filter cutoff frequency that is applied to our elevator setpoint.  Lower Hz = more delay on stop.

} pidProfile_t;

PG_DECLARE_ARRAY(pidProfile_t, PID_PROFILE_COUNT, pidProfiles);

typedef struct pidConfig_s {
    uint8_t pid_process_denom;              // Processing denominator for PID controller vs gyro sampling rate
    uint16_t collective_reference;          // Mixer stabilized collective output that gives 8 degrees of collective when mixScales[SC] = 1000
} pidConfig_t;

PG_DECLARE(pidConfig_t, pidConfig);

union rollAndPitchTrims_u;
void pidController(const pidProfile_t *pidProfile, timeUs_t currentTimeUs);

typedef struct pidAxisData_s {
    float P;
    float I;
    float D;
    float F;
    float Sum;
    float SumLim;
} pidAxisData_t;

extern const char pidNames[];

extern pidAxisData_t pidData[3];

extern uint32_t targetPidLooptime;

void pidResetIterm(void);
void pidInitFilters(const pidProfile_t *pidProfile);
void pidInitConfig(const pidProfile_t *pidProfile);
void pidInit(const pidProfile_t *pidProfile);
void pidCopyProfile(uint8_t dstPidProfileIndex, uint8_t srcPidProfileIndex);
void pidAcroTrainerInit(void);
void pidSetAcroTrainerState(bool newState);
void pidInitSetpointDerivativeLpf(uint16_t filterCutoff, uint8_t debugAxis, uint8_t filterType);
void pidUpdateSetpointDerivativeLpf(uint16_t filterCutoff);
#ifdef USE_THRUST_LINEARIZATION
float pidApplyThrustLinearization(float motorValue);
float pidCompensateThrustLinearization(float throttle);
#endif

#ifdef UNIT_TEST
#include "sensors/acceleration.h"
extern float axisError[XYZ_AXIS_COUNT];
void applyItermRelax(const int axis, const float iterm,
    const float gyroRate, float *itermErrorRate, float *currentPidSetpoint);
void applyAbsoluteControl(const int axis, const float gyroRate, float *currentPidSetpoint, float *itermErrorRate);
void rotateItermAndAxisError();
float pidLevel(int axis, const pidProfile_t *pidProfile,
    const rollAndPitchTrims_t *angleTrim, float currentPidSetpoint);
float calcHorizonLevelStrength(void);
#endif
void dynLpfDTermUpdate(float throttle);
float pidGetPreviousSetpoint(int axis);
float pidGetDT();
float pidGetPidFrequency();
float pidGetFfBoostFactor();
float pidGetFfSmoothFactor();
float pidGetSpikeLimitInverse();
float dynDtermLpfCutoffFreq(float throttle, uint16_t dynLpfMin, uint16_t dynLpfMax, uint8_t expo);
float getCollectiveDeflectionAbs();
float getCollectiveDeflectionAbsHPF();
