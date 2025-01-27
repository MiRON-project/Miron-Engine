#include "minizinc.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <typeinfo>
#include <change_velocity.hpp>
#include "variant_client.hpp"
#include "query_client.hpp"
#include "abort_current_skill.hpp"

using namespace std;
using namespace zmqserver;

Minizinc::Minizinc() {
	solutions = new Solution();
	mzn_path_ = "";
	dzn_path_ = "";
}
Minizinc::Minizinc(std::string mzn_path,std::string dzn_path,
		std::vector<DataPair> parameters, std::vector<DataPair> varPoints) {
	solutions = new Solution();
	solutions->msg = "";
	solutions->output = { };
	variant_client_ = std::make_shared<VariantClient>();
	query_client_ = std::make_shared<QueryClient>();
	mzn_path_ = mzn_path;
	dzn_path_ = dzn_path;
	inputs = parameters;
	DataPair dp;
	//Create the buffer
	for (int i = 0; i < varPoints.size(); ++i) {
		buffer.push_back(varPoints[i]);
	}
	//solutions->output=buffer;
	for (int i = 0; i < buffer.size(); ++i) {
		solutions->output.push_back(buffer[i]);
	}

	FILE *fp;
	char output[1035];
	bool error = false;
	std::string command;

	// Checking minizinc command
	command = "minizinc --version 2>&1 1>/dev/null";
	//cout << "Command: "<< command << endl;
	fp = popen(command.c_str(), "r");

	if (fp == NULL) {
		std::cerr << "[Minizinc]" << std::endl;
		std::cerr
				<< "\tAn error has been encountered while checking the minizinc command"
				<< std::endl;
		error = true;
	} else {
		while (fgets(output, sizeof(output) - 1, fp) != NULL) {
			printf("[Minizinc] \t%s", output);
			error = true;
		}
		pclose(fp);
	}
	if (error)
		exit(EXIT_FAILURE);
}

Minizinc::~Minizinc() {
}

bool Minizinc::is_empty(std::ifstream &pFile) {
	return pFile.good();
}

void Minizinc::ParseSolution() {
	std::string msg = solutions->msg;
	size_t pos1 = 0;
	size_t pos2 = 0;
	std::string assignment_token;
	std::string assignment_delimiter = ";";
	std::string blankspace_delimiter = " ";
	std::string param_delimiter = "=";
	DataPair varpoint;
	vector<DataPair> newSolutions;

	// Tokinizes the string
	while ((pos1 = msg.find(assignment_delimiter)) != std::string::npos) {
		assignment_token = msg.substr(0, pos1);
		if ((pos2 = assignment_token.find(param_delimiter))
				!= std::string::npos) {
			varpoint.id = assignment_token.substr(0, pos2);
			varpoint.value = assignment_token.substr(pos2 + 1,
					assignment_token.length());
			//Check previous value
			newSolutions.push_back(varpoint);
		}
		msg.erase(0, pos1 + assignment_delimiter.length());
	}

	checkSolutions(newSolutions);
}
void Minizinc::abortCurrentSkill(double value)
{
	ApproachDist app_dist(100000);
	AbortCurrentSkill abort_msg(query_client_->getID(), app_dist);
	query_client_->setMsg(std::move(abort_msg.dump()));
	query_client_->send();
}
void Minizinc::checkSolutions(vector<DataPair> newSolutions) {

	for (int i = 0; i < solutions->output.size(); ++i) {
		for (int j = 0; j < newSolutions.size(); ++j) {
			if (solutions->output[i].id == newSolutions[j].id) {
				if (solutions->output[i].value != newSolutions[j].value) {
					solutions->output[i].value = newSolutions[j].value;
					if (solutions->output[i].type == "BehaviorTreeEnumType") {	//Check type of VariantPoint
						cout << "Change variant and send through ZMQ" << endl;
						variant_client_->sendVariant(newSolutions[j].value);
						abortCurrentSkill(100000);
					} else {
						Parameter vel(stoi(solutions->output[i].value),
								stoi(solutions->output[i].value));
						ChangeP vel_msg(query_client_->getID(), solutions->output[i].smartsoftinfo, vel);
						query_client_->setMsg(std::move(vel_msg.dump()));
						query_client_->send();
					}

				}
			}
		}
	}
}

int Minizinc::Run() {
	return Run(false);
}

int Minizinc::Run(bool display_msgs) {
	FILE *fp;
	char output_str[1035];
	std::string command;
	std::string context_str;
	std::string solution_str;
	int result = 0;

	cout << "RUN " << endl;
	// Getting the parameters as string
	context_str = "";
	for (unsigned int i = 0; i < inputs.size(); i++) {
		context_str.append(inputs[i].id.c_str());
		context_str.append("=");
		context_str.append(inputs[i].value.c_str());
		context_str.append(";");
	}
	cout << "Context str " << context_str << endl;

	// Running minizinc command to execute the optimization
	if (result == 0) {
		command = "minizinc -D \"" + context_str + "\" " + mzn_path_ + " "
				+ dzn_path_;
		/*+ mzn_path_.substr(0,mzn_path_.size()-3) + "ozn --output-to-file "
		 + mzn_path_.substr(0,mzn_path_.size()-3) + "fzn\"";*/

		//Print minizinc command
		//cout << "Command new:" << command << endl;
		fp = popen(command.c_str(), "r");
		if (fp == NULL) {
			std::cerr << "[Minizinc.run]" << std::endl;
			std::cerr
					<< "\tAn error has been encountered while executing minizinc"
					<< std::endl;
			result = -1;

		} else {

			// Getting the solution
			while (fgets(output_str, sizeof(output_str) - 1, fp) != NULL) {
				solution_str.append(output_str);
				//Remove blank spaces before write into a txt file
				solution_str = removeSpaces(solution_str);
				if (display_msgs) {
					printf("[Minizinc.run] \t%s", output_str);
				}
			}

			solutions->msg = solution_str;
			ParseSolution();
			pclose(fp);
		}
	}

	return result;
}

void Minizinc::updateParameter(std::string param, std::string value) {
	
	cout << endl;
	cout << "Actualizacion de parametro" << endl;
	for (int i = 0; i < inputs.size(); ++i) {
		cout << "Param: " << inputs[i].id <<":"<< inputs[i].value << endl;
		if (inputs[i].id == param) {
			inputs[i].value = value;
			cout << "Actualizamos el valor de "<< param << " a " << value << endl;
		}
	}
}

string Minizinc::removeSpaces(string str) {
	str.erase(remove(str.begin(), str.end(), ' '), str.end());
	return str;
}

