
%
% Metrics definitions
%

float : metric_RoqmeSystemWebotsIntralogistics_safety;
float : metric_RoqmeSystemWebotsIntralogistics_mission_completion;
float : metric_RoqmeSystemWebotsIntralogistics_power_autonomy;

	% Weights
	float : weight_RoqmeSystemWebotsIntralogistics_safety = metric_RoqmeSystemWebotsIntralogistics_safety;		
	float : weight_RoqmeSystemWebotsIntralogistics_mission_completion = metric_RoqmeSystemWebotsIntralogistics_mission_completion;		

%
% Definitions for task
%
	
% Fixed value support
bool  : task_input;
float : task_input_value;
	
% Number of values
int : task_n;
	
% Index definition
set of int: task_index = 1..task_n;
	
% X-Axis values
array [task_index] of float: task_x;

% X-Axis labels
enum task_lbl; 
	
% Decision variable
var 1..task_n: task_var;

% Constraints
constraint task_input -> task_x[task_var] = task_input_value;

%
% Definitions for maxVelocity
%
	
% Fixed value support
bool  : maxVelocity_input;
float : maxVelocity_input_value;
	
% Number of values
int : maxVelocity_n;
	
% Index definition
set of int: maxVelocity_index = 1..maxVelocity_n;
	
% X-Axis values
array [maxVelocity_index] of float: maxVelocity_x;


	% Y-Axis values (utility)
	array [maxVelocity_index] of float: maxVelocity_y_RoqmeSystemWebotsIntralogistics_safety;
	array [maxVelocity_index] of float: maxVelocity_y_RoqmeSystemWebotsIntralogistics_mission_completion;
	
% Decision variable
var 1..maxVelocity_n: maxVelocity_var;

% Constraints
constraint maxVelocity_input -> abs(maxVelocity_x[maxVelocity_var] - maxVelocity_input_value) < 0.2261;

%
% MIRoN rules
%
		
	% Rule noBattery
	constraint not(task_input) /\ metric_RoqmeSystemWebotsIntralogistics_power_autonomy < 0.3 -> task_x[task_var] = 2;

%
% Solver objective
%
	
solve maximize
	weight_RoqmeSystemWebotsIntralogistics_safety * maxVelocity_y_RoqmeSystemWebotsIntralogistics_safety[maxVelocity_var] + 
	weight_RoqmeSystemWebotsIntralogistics_mission_completion * maxVelocity_y_RoqmeSystemWebotsIntralogistics_mission_completion[maxVelocity_var];

%
% Pretty-printed output
%

output [
	"task = ", show(task_lbl[task_var]),"; ", 
	"maxVelocity = ", show(maxVelocity_x[maxVelocity_var]),"; "];
