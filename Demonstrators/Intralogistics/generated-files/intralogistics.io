 { "inputs" : [
 	{"mzn_id":"metric_RoqmeSystemWebotsIntralogistics_safety", "class":"Property", "name":"safety", "namespace":"RoqmeSystemWebotsIntralogistics", "datatype":"NumericType"}, 
 	{"mzn_id":"metric_RoqmeSystemWebotsIntralogistics_mission_completion", "class":"Property", "name":"mission_completion", "namespace":"RoqmeSystemWebotsIntralogistics", "datatype":"NumericType"}, 
 	{"mzn_id":"metric_RoqmeSystemWebotsIntralogistics_power_autonomy", "class":"Property", "name":"power_autonomy", "namespace":"RoqmeSystemWebotsIntralogistics", "datatype":"NumericType"}, 
 	{"mzn_id":"task_input", "datatype":"BooleanType", "class":"", "name":"", "namespace":""},
 	{"mzn_id":"task_input_value", "class":"VariationPoint", "name":"task", "namespace":"", "datatype":"BehaviorTreeEnumType"}, 
 	{"mzn_id":"maxVelocity_input", "datatype":"BooleanType", "class":"", "name":"", "namespace":""},
 	{"mzn_id":"maxVelocity_input_value", "class":"VariationPoint", "name":"maxVelocity", "namespace":"", "datatype":"NumericSampledType"}
 ], "outputs" : [
 	{"mzn_id":"task", "class":"VariationPoint", "name":"task", "namespace":"", "datatype":"BehaviorTreeEnumType", "smartsoft":{}}, 
 	{"mzn_id":"maxVelocity", "class":"VariationPoint", "name":"maxVelocity", "namespace":"", "datatype":"NumericSampledType", "smartsoft":{"parameterSetRepository":"CommNavigationObjects", "parameterSet":"CdlParameter", "component":"SmartCdlServer", "parameter":"transvel"}}
 ]}
