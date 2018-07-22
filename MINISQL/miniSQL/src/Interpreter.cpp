#include "Interpreter.h"
#include <stdexcept>
#include <cstring>

using namespace std;

Interpreter::Interpreter(){
	//default ctor
}

Interpreter::~Interpreter(){
	//default dtor
}

int Interpreter::interpret(string s){
    int tmp = 0;
	string command;

	command = getCommand(s, tmp);

	//create operation
	if(command == "create"){
        command = getCommand(s, tmp);

		if(command == "table"){
			string primaryKey = "";
			string tableName = "";
			command = getCommand(s, tmp);
			if(!command.empty())			//create table tablename
				tableName = command;
			else{
				cout << "Syntax Error! No table name" << endl;
				return 0;
			}

			command = getCommand(s, tmp);
			if(command.empty() || command != "("){
				cout << "Syntax Error! Miss (" << endl;
				return 0;
			}
			else{
				command = getCommand(s, tmp);
				vector<Attribute> attributeVector;
				while (!command.empty() && command != "primary"
                        && command != ")"){
					string attributeName = command;
					int type = 0;
					bool ifUnique = false;
					// deal with the data type
					command = getCommand(s, tmp);
					if(command == "int"){
						type = 0;
                    }
					else if(command == "float"){
						type = -1;
                    }
					else if(command == "char"){
						command = getCommand(s, tmp);
						if (command != "("){
							cout << "Syntax Error: unknown data type" << endl;
							return 0;
						}
						command = getCommand(s, tmp);
						istringstream convert(command);
						if (!(convert >> type)){
							cout << "Syntax error : illegal number in char()" << endl;
							return 0;
						}
						command = getCommand(s, tmp);
						if (command != ")"){
							cout << "Syntax Error: unknown data type" << endl;
							return 0;
						}
					}
					else{
						cout << "Syntax Error! Unknown data type" << endl;
						return 0;
					}
					command = getCommand(s, tmp);
					if(command == "unique"){
						ifUnique = true;
                        command = getCommand(s, tmp);
					}
					Attribute attribute(attributeName,type,ifUnique);
					attributeVector.push_back(attribute);
					if(command != ","){
						if(command != ")"){
							cout << "Syntax Error!" << endl;
							return 0;
						}
						else break;
					}

					command = getCommand(s, tmp);
				}

				int primaryKeyLocation = 0;
				if(command == "primary"){
					command = getCommand(s, tmp);
					if(command != "key"){
						cout << "Error in syntax!" << endl;
						return 0;
					}
					else{
						command = getCommand(s, tmp);
						if(command == "("){
							command = getCommand(s, tmp);
							primaryKey = command;
							int i = 0;
							for( i = 0; i < attributeVector.size(); i++){
								if(primaryKey == attributeVector[i].getName()){
									attributeVector[i].setUnique(true);
									break;
								}

							}
							if(i == attributeVector.size()){
								cout << "Syntax Error! primary key does not exist" << endl;
								return 0;
							}
							primaryKeyLocation = i;
							command = getCommand(s, tmp);
							if(command != ")"){
								cout << "Syntax Error!" << endl;
								return 0;
							}
						}
						else{
							cout << "Syntax Error!" << endl;
							return 0;
						}
						command = getCommand(s, tmp);
						if(command != ")"){
							cout << "Syntax Error" << endl;
							return 0;
						}
					}
				}
				else if(command.empty()){
					cout << "Syntax Error!" << endl;
					return 0;
				}

				api->createTable(tableName, &attributeVector, primaryKey, primaryKeyLocation);
				return 1;
			}
		}
		else if(command == "index")
		{
			string indexName = "";
			string tableName = "";
			string attributeName = "";
			command = getCommand(s, tmp);
			if(!command.empty()){			//create index indexname
				indexName = command;
            }
			else{
				cout << "Syntax Error!" << endl;
				return 0;
			}

			command = getCommand(s, tmp);
			try {
				logic_error ex("Syntax Error!");
				if(command != "on")
					throw exception(ex);
				command = getCommand(s, tmp);
				if(command.empty())
					throw exception(ex);
				tableName = command;
				command = getCommand(s, tmp);
				if(command != "(")
					throw exception(ex);
				command = getCommand(s, tmp);
				if(command.empty())
					throw exception(ex);
				attributeName = command;
				command = getCommand(s, tmp);
				if(command != ")")
					throw exception(ex);
				api->createIndex(indexName, tableName, attributeName);
				return 1;
			} 
        catch(exception&) {
				cout << "Syntax Error!" << endl;
				return 0;
			}
		}
		else{
			cout << "Syntax Error!" << endl;
			return 0;
		}
		return 0;
	}

	//select operation
	else if(command == "select"){
		vector<string> attrSelected;
		string tableName = "";
		command = getCommand(s, tmp);
		if(command != "*"){
			while(command != "from"){
				attrSelected.push_back(command);
				command = getCommand(s, tmp);
			}
		}
		else{
			command = getCommand(s, tmp);
		}
		if(command != "from"){
			cout << "Syntax Error!" << endl;
			return 0;
		}

		command = getCommand(s, tmp);
		if(!command.empty())
			tableName = command;
		else{
			cout << "Syntax Error!" << endl;
			return 0;
		}

		// condition extricate
		command = getCommand(s, tmp);
		if(command.empty()){
			if(attrSelected.size() == 0){
				api-> showRecord(tableName);
			}
			else{
				api-> showRecord(tableName, &attrSelected);
			}
			return 1;
		}
		else if(command == "where"){
			string attributeName = "";
			string value = "";
			int operate = Condition::OPERATOR_EQUAL;
			std::vector<Condition> conditionVector;
			command = getCommand(s, tmp);
			while(1){
				try {
					logic_error ex("Syntax Error!");
					logic_error ex2("Syntax Error! No operator!");
					logic_error ex3("Syntax Error! No value!");
					if(command.empty())
						throw exception(ex);
					attributeName = command ;
					command = getCommand(s, tmp);
					if(command == "<=")
						operate = Condition::OPERATOR_LESS_EQUAL;
					else if(command == ">=")
						operate = Condition::OPERATOR_MORE_EQUAL;
					else if(command == "<")
						operate = Condition::OPERATOR_LESS;
					else if(command == ">")
						operate = Condition::OPERATOR_MORE;
					else if(command == "=")
						operate = Condition::OPERATOR_EQUAL;
					else if(command == "<>")
						operate = Condition::OPERATOR_NOT_EQUAL;
					else
						throw exception(ex2);
					command = getCommand(s, tmp);
					if(command.empty()) // no condition
						throw exception(ex3);
					value = command;
					Condition c(attributeName, value, operate);
					conditionVector.push_back(c);
					command = getCommand(s, tmp);
					if(command.empty()) // no condition
						break;
					if(command != "and")
						throw exception(ex);
					command = getCommand(s, tmp);
				} 	
				catch (exception& e) {
					cout << e.what() << endl;
					return 0;
				}
			}
			if(attrSelected.size()==0)
				api-> showRecord(tableName, NULL, &conditionVector);
			else
				api->showRecord(tableName, &attrSelected, &conditionVector);
			
			return 1;
		}
	}

	//drop operation
	else if(command == "drop"){
		command = getCommand(s, tmp);

		if(command == "table"){
			command = getCommand(s, tmp);
			if(!command.empty()){
				api->dropTable(command);
				return 1;
			}
			else{
				cout << "Syntax Error!" << endl;
				return 1;
			}
		}
		else if(command == "index"){
			command = getCommand(s, tmp);
			if(!command.empty()){
				api->dropIndex(command);
				return 1;
			}
			else{
				cout << "Syntax Error!" << endl;
				return 1;
			}
		}
		else{
			cout << "Syntax Error!" << endl;
			return 0;
		}
	}

	//delete operation
	else if(command == "delete"){
		string tableName = "";
		command = getCommand(s, tmp);
		if(command != "from"){
			cout << "Syntax Error!" << endl;
			return 0;
		}

		command = getCommand(s, tmp);
		if(!command.empty())
			tableName = command;
		else{
			cout << "Syntax Error!" << endl;
			return 0;
		}

		// condition extricate
		command = getCommand(s, tmp);
		if(command.empty()){
			api->deleteRecord(tableName);
			return 1;
		}
		else if(command == "where"){
			string attributeName = "";
			string value = "";
			int operate = Condition::OPERATOR_EQUAL;
			std::vector<Condition> conditionVector;
			command = getCommand(s, tmp);		//col1
			while(1){
				try {
					logic_error ex("Syntax Error!");
					if(command.empty())
						throw exception(ex);
					attributeName = command ;
					command = getCommand(s, tmp);
					if(command == "<=")
						operate = Condition::OPERATOR_LESS_EQUAL;
					else if(command == ">=")
						operate = Condition::OPERATOR_MORE_EQUAL;
					else if(command == "<")
						operate = Condition::OPERATOR_LESS;
					else if(command == ">")
						operate = Condition::OPERATOR_MORE;
					else if(command == "=")
						operate = Condition::OPERATOR_EQUAL;
					else if(command == "<>")
						operate = Condition::OPERATOR_NOT_EQUAL;
					else
						throw exception(ex);
					command = getCommand(s, tmp);
					if(command.empty()) // no condition
						throw exception(ex);
					value = command;
					command = getCommand(s, tmp);
					Condition c(attributeName, value, operate);
					conditionVector.push_back(c);
					if(command.empty()) // no condition
						break;
					if(command != "and")
						throw  exception(ex);;
					command = getCommand(s, tmp);
				} 	catch (exception&) {
					cout << "Syntax Error!" << endl;
					return 0;
				}
			}
			api->deleteRecord(tableName, &conditionVector);
			return 1;
		}
	}

	//insert operation
	else if(command == "insert"){
		string tableName = "";
		std::vector<string> valueVector;
		command = getCommand(s, tmp);
		try {
			logic_error ex("Syntax Error!");
			if(strcmp(command.c_str(),"into") != 0)
				throw  exception(ex);
			command = getCommand(s, tmp);
			if(command.empty())
				throw  exception(ex);
			tableName = command;
			command = getCommand(s, tmp);
			if(command != "values")
				throw  exception(ex);
			command = getCommand(s, tmp);
			if(command != "(")
				throw  exception(ex);
			command = getCommand(s, tmp);
			while (!command.empty() && command != ")"){
				valueVector.push_back(command);
				command = getCommand(s, tmp);
				if(command == ",")  // bug here
					command = getCommand(s, tmp);
			}
			if(command != ")")
				throw  exception(ex);
		} catch (exception&){
			cout << "Syntax Error!" << endl;
			return 0;
		}
		api->insertRecord(tableName, &valueVector);
		return 1;
	}

	//quit minisql program
	else if(command == "quit" || command == "exit"){ 
		return -1;
	}

	//help information
	else if (command == "help") {
		return -2;
	}

	//execute operation
	else if(command == "execfile"){
		fileName = getCommand(s, tmp);
		cout << "try to open file " << fileName << "." << endl;
		return 2;
	}

	else{
		if(command != "")
			cout << "Error, command " << command << " not found" << endl;
		return 0;
	}
	return 0;
}

string Interpreter::getCommand(string s, int &tmp){
    string command;
    int index1, index2;

	//get the first index of the word
	while ((s[tmp] == ' ' || s[tmp] == '\n'  || s[tmp] == '\t') && s[tmp] != 0)
		tmp++;
	index1 = tmp;

	//get the next index of the word
	if(s[tmp] == '(' || s[tmp] == ',' || s[tmp] == ')'){
		tmp++;
		index2 = tmp;
		command = s.substr(index1, index2 - index1);
	}

	else if(s[tmp] == '\''){
		tmp++;
		while (s[tmp] != '\'' && s[tmp] !=0)
			tmp++;

		if(s[tmp] == '\''){
			index1++;
			index2 = tmp;
			tmp++;
			command = s.substr(index1, index2 - index1);
		}
		else
			command = "";
	}

	else{
		while (s[tmp] != ' ' &&s[tmp] != '(' && s[tmp] != '\n' 
                && s[tmp] != 0 && s[tmp] != ')' && s[tmp] != ',')
			tmp++;

		index2 = tmp;
		if(index1 != index2)
			command = s.substr(index1, index2 - index1);

        else
			command = "";
	}

    return command;
}

string Interpreter::getFilename() {
	return fileName;
}

void Interpreter::setAPI(API* apiInput) {
	api = apiInput;
}
