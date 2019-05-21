#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <cmath>
#include "DEF_Util.h"

using namespace std;

map <string, int> metalTable;
map <string, int> cutTable;
map <string, macro> macros;
map <string, component> components;

vector <metal> metalLayers;
vector <cut> cutLayers;
vector <via> vias;
vector <RECT> obs;
vector<vector<vector<RECT> > > nets;
vector <int> order;

string inDEF;

DEF DEF_FILE;
LEF LEF_FILE;


vector<string> Extract(string line){
    int l = 0, r;
    while(l < line.length() && line[l] == ' ' || line[l] == '\t')l++;
    r = l;
    vector<string> tokens;

    while(l < line.length()){
        while(r < line.length() && line[r] != ' ' && line[r] != '\t') r++;
        tokens.push_back(line.substr(l, r - l));
        while(r < line.length() && line[r] == ' ' || line[r] == '\t') r++;
        l = r;
    }
    return tokens;
}

RECT fix(RECT rect){
    RECT fixd;

    fixd.metalLayer = rect.metalLayer;
    fixd.x1 = min(rect.x1, rect.x2);
    fixd.y1 = min(rect.y1, rect.y2);
    fixd.x2 = max(rect.x1, rect.x2);
    fixd.y2 = max(rect.y1, rect.y2);

    return fixd;
}


void readComponents(ifstream &read, int num){


    string buf;
    vector <string> Buf;
    int done = 0;

    getline(read, buf);
    Buf = Extract(buf);
    while(!(Buf.size() == 2 && Buf[0] == "END" && Buf[1] == "COMPONENTS")){

        if(Buf.size() == 11 && Buf[0] == "-" && Buf[3] == "+" && Buf[4] == "PLACED" && Buf[5] == "(" && Buf[8] == ")" && Buf[10] == ";"){
            done++;

            component newComponent;
            newComponent.instance = Buf[1];
            newComponent.macro = Buf[2];
            newComponent.x_pos = stoi(Buf[6]);
            newComponent.y_pos = stoi(Buf[7]);
            newComponent.orientation = Buf[9];

            components[Buf[1]] = newComponent;

            //Adding the obstructions
            macro placed = macros[Buf[2]];
            for(int i = 0; i < placed.obs.size(); i++){
                RECT obstruction;
                obstruction.metalLayer = placed.obs[i].metalLayer;
                if(Buf[9] == "N"){
                    obstruction.x1 = placed.obs[i].x1 + newComponent.x_pos;
                    obstruction.y1 = placed.obs[i].y1 + newComponent.y_pos;
                    obstruction.x2 = placed.obs[i].x2 + newComponent.x_pos;
                    obstruction.y2 = placed.obs[i].y2 + newComponent.y_pos;
                }
                else if(Buf[9] == "FN"){
                    obstruction.x1 = (placed.width - placed.obs[i].x1) + newComponent.x_pos;
                    obstruction.y1 = placed.obs[i].y1 + newComponent.y_pos;
                    obstruction.x2 = (placed.width - placed.obs[i].x2) + newComponent.x_pos;
                    obstruction.y2 = placed.obs[i].y2 + newComponent.y_pos;
                }
                else if(Buf[9] == "FS"){
                    obstruction.x1 = placed.obs[i].x1 + newComponent.x_pos;
                    obstruction.y1 = (placed.height - placed.obs[i].y1) + newComponent.y_pos;
                    obstruction.x2 = placed.obs[i].x2 + newComponent.x_pos;
                    obstruction.y2 = (placed.height - placed.obs[i].y2) + newComponent.y_pos;
                }
                else if(Buf[9] == "S"){
                    obstruction.x1 = (placed.width - placed.obs[i].x1) + newComponent.x_pos;
                    obstruction.y1 = (placed.height - placed.obs[i].y1) + newComponent.y_pos;
                    obstruction.x2 = (placed.width - placed.obs[i].x2) + newComponent.x_pos;
                    obstruction.y2 = (placed.height - placed.obs[i].y2) + newComponent.y_pos;
                }
                obs.push_back(fix(obstruction));
            }
        }
        getline(read, buf);
        Buf = Extract(buf);
    }

    if(done != num){
        cout << "Warning: The components list is not equal to " << num << " as stated at the beginning of the components section\n";
    }
}

void readNets(ifstream &read, int num){
    string buf;
    vector <string> Buf;

    getline(read, buf);
    Buf = Extract(buf);
    int index = -1;
    while(!(Buf.size() == 2 && Buf[0] == "END" && Buf[1] == "NETS")){
        if(Buf.size() == 2 && Buf[0] == "-"){
            index++;
            vector<pair<string, string> > pins;
            bool routable = 1;
            do{
                getline(read, buf);
                Buf = Extract(buf);
                if(Buf.size() >= 4 && Buf[0] == "(" && Buf[3] == ")"){
                    if(Buf[1] == "PIN")routable = 0;
                    pins.push_back(make_pair(Buf[1], Buf[2]));
                }
            }while(!(Buf.size() == 5 && Buf[4] == ";"));



            if(routable){

                order.push_back(index);

                vector<vector<RECT> > pins_temp;
                vector<RECT> rects_temp;

                for(int i = 0; i < pins.size(); i++){
                    rects_temp = macros[components[pins[i].first].macro].pins[pins[i].second].ports;
                    int x_pos = components[pins[i].first].x_pos;
                    int y_pos = components[pins[i].first].y_pos;
                    int width = macros[components[pins[i].first].macro].width;
                    int height = macros[components[pins[i].first].macro].height;
                    string orient = components[pins[i].first].orientation;

                    for(int j = 0; j < rects_temp.size(); j++){

                        if(orient == "N"){
                            rects_temp[j].x1 = rects_temp[j].x1 + x_pos;
                            rects_temp[j].y1 = rects_temp[j].y1 + y_pos;
                            rects_temp[j].x2 = rects_temp[j].x2 + x_pos;
                            rects_temp[j].y2 = rects_temp[j].y2 + y_pos;
                        }
                        else if(orient == "FN"){
                            rects_temp[j].x1 = (width - rects_temp[j].x1) + x_pos;
                            rects_temp[j].y1 = rects_temp[j].y1 + y_pos;
                            rects_temp[j].x2 = (width - rects_temp[j].x2) + x_pos;
                            rects_temp[j].y2 = rects_temp[j].y2 + y_pos;
                        }
                        else if(orient == "FS"){
                            rects_temp[j].x1 = rects_temp[j].x1 + x_pos;
                            rects_temp[j].y1 = (height - rects_temp[j].y1) + y_pos;
                            rects_temp[j].x2 = rects_temp[j].x2 + x_pos;
                            rects_temp[j].y2 = (height - rects_temp[j].y2) + y_pos;
                        }
                        else if(orient == "S"){
                            rects_temp[j].x1 = (width - rects_temp[j].x1) + x_pos;
                            rects_temp[j].y1 = (height - rects_temp[j].y1) + y_pos;
                            rects_temp[j].x2 = (width - rects_temp[j].x2) + x_pos;
                            rects_temp[j].y2 = (height - rects_temp[j].y2) + y_pos;
                        }

                        rects_temp[j] = fix(rects_temp[j]);
                    }
                    pins_temp.push_back(rects_temp);
                }
                nets.push_back(pins_temp);
            }

        }
        getline(read, buf);
        Buf = Extract(buf);
    }


}

bool readDEF(){
	string source, buf;
	vector <string> Buf;
	cout << "Enter the path for the .def file: ";
	getline(cin, source);
    inDEF = source;
	ifstream read(source);
	if(!read.is_open()){
	  cout << "Error: Unable to read the .def file.\n";
	  return 0;
	}
	int line = 1;
	getline(read, buf);
	while(!read.eof()){
		Buf = Extract(buf);
		int index = 0;
		while(index < Buf.size()){
			if(Buf[index] == "VERSION"){
				if(Buf.size() > index + 2 && Buf[index + 2] == ";"){
					DEF_FILE.ver = Buf[index + 1];
					index += 3;
				}
				else {
                    cout << "Undefined parameters list after \"VERSION\"\n";
                    return 0;
				}
			}

			else if(Buf[index] == "DIVIDERCHAR"){
				if(Buf.size() > index + 2 && Buf[index + 2] == ";" && Buf[index + 1].length() == 3 && Buf[index + 1][0] == '\"' && Buf[index + 1][2] == '\"'){
					DEF_FILE.div = Buf[index + 1][1];
					index += 3;
				}
				else {
                    cout << "Undefined parameters list after \"DIVIDERCHAR\"\n";
                    return 0;
				}
			}

            else if(Buf[index] == "BUSBITCHARS"){
                if(Buf.size() > index + 2 && Buf[index + 2] == ";" && Buf[index + 1].length() == 4 && Buf[index + 1][0] == '\"' && Buf[index + 1][3] == '\"'){
                    DEF_FILE.bus_left = Buf[index + 1][1];
                    DEF_FILE.bus_right = Buf[index + 1][2];
                    index += 3;
                }
                else {
                    cout << "Undefined parameters list after \"BUSBITCHARS\"\n";
                    return 0;
                }
            }

            else if(Buf[index] == "DESIGN"){
				if(Buf.size() > index + 2 && Buf[index + 2] == ";"){
					DEF_FILE.design = Buf[index + 1];
					index += 3;
				}
				else {
                    cout << "Undefined parameters list after \"DESIGN\"\n";
                    return 0;
				}
            }

            else if(Buf[index] == "TECHNOLOGY"){
				if(Buf.size() > index + 2 && Buf[index + 2] == ";"){
					DEF_FILE.tech = Buf[index + 1];
					index += 3;
				}
				else {
                    cout << "Undefined parameters list after \"TECHNOLOGY\"\n";
                    return 0;
				}
            }

            else if(Buf[index] == "UNITS"){
                if(Buf.size() > index + 4 && Buf[index + 1] == "DISTANCE" && Buf[index + 2] == "MICRONS" && Buf[index + 4] == ";"){
                    DEF_FILE.unit = stoi(Buf[index + 3]);
                    index += 5;
                }
                else {
                    cout << "Undefined parameters list after \"UNITS\"\n";
                    return 0;
                }
            }

            else if(Buf[index] == "DIEAREA"){
                int points = 0;
                while(Buf.size() > index + 4){
                    if(Buf[index + 1] == "(" && Buf[index + 4] == ")"){
                        DEF_FILE.dieArea.push_back(make_pair(stoi(Buf[index + 2]), stoi(Buf[index + 3])));
                    }
                    else{
                        cout << "Undefined parameters list after \"UNITS\"\n";
                        return 0;
                    }
                    index += 4;
                }
                if(!(Buf.size() > index + 1 && Buf[index + 1] == ";")){
                    cout << "Undefined parameters list after \"UNITS\"\n";
                    return 0;
                }
                index += 2;

			}

			else if(Buf[index] == "TRACKS"){
               if(Buf.size() > index + 9 && Buf[index + 3] == "DO" && Buf[index + 5] == "STEP" && Buf[index + 7] == "LAYER" && Buf[index + 9] == ";"){
                    int metal_index = metalTable[Buf[8]];

                    metalLayers[metal_index].start = stod(Buf[2]);
                    metalLayers[metal_index].numTracks = stoi(Buf[4]);
                    metalLayers[metal_index].step = stod(Buf[6]);

                    index += 10;

                }
                else {
                    cout << "Undefined parameters list after \"TRACKS\"\n";
                    return 0;
                }
			}
			else if(Buf.size() > index + 2 && Buf[index] == "COMPONENTS" && Buf[index + 2] == ";"){
                readComponents(read, stod(Buf[1]));

                index = Buf.size();
			}
			else if(Buf.size() > index + 2 && Buf[index] == "NETS" && Buf[index + 2] == ";"){
                readNets(read, stod(Buf[1]));

                index = Buf.size();
			}
			else index = Buf.size();
		}
    getline(read, buf);
	}
	return 1;

}

void readObs(ifstream &read, macro &newMacro){

    string buf;
    vector <string> Buf;

    getline(read, buf);
    Buf = Extract(buf);
    int layer = -1;

    while(!(Buf.size() == 1 && Buf[0] == "END")){
        if(Buf.size() == 3 && Buf[0] == "LAYER" && Buf[2] == ";"){
            if(metalTable.find(Buf[1]) != metalTable.end()) layer = metalTable[Buf[1]];
            else layer = -1;
        }
        else if(Buf.size() == 6 && Buf[0] == "RECT" && Buf[5] == ";" && layer > -1){
            RECT newRECT;
            newRECT.metalLayer = layer;
            newRECT.x1 = round(stod(Buf[1]) * 100);
            newRECT.y1 = round(stod(Buf[2]) * 100);
            newRECT.x2 = round(stod(Buf[3]) * 100);
            newRECT.y2 = round(stod(Buf[4]) * 100);
            newMacro.obs.push_back(newRECT);
        }
        getline(read, buf);
        Buf = Extract(buf);
    }
}

void readPin(ifstream &read, string name, macro &newMacro){
    string buf;
    vector <string> Buf;

    pin newPin;
    newPin.label = name;
    getline(read, buf);
    Buf = Extract(buf);

    while(!(Buf.size() == 1 && Buf[0] == "PORT")){
        getline(read, buf);
        Buf = Extract(buf);
    }

    int layer = -1;

    getline(read, buf);
    Buf = Extract(buf);
    while(!(Buf.size() == 1 && Buf[0] == "END")){
        if(Buf.size() == 3 && Buf[0] == "LAYER" && Buf[2] == ";"){
            if(metalTable.find(Buf[1]) != metalTable.end()) layer = metalTable[Buf[1]];
            else layer = -1;
        }
        else if(Buf.size() == 6 && Buf[0] == "RECT" && Buf[5] == ";" && layer > -1){
            RECT newRECT;
            newRECT.metalLayer = layer;
            newRECT.x1 = round(stod(Buf[1]) * 100);
            newRECT.y1 = round(stod(Buf[2]) * 100);
            newRECT.x2 = round(stod(Buf[3]) * 100);
            newRECT.y2 = round(stod(Buf[4]) * 100);
            newPin.ports.push_back(newRECT);
        }
        getline(read, buf);
        Buf = Extract(buf);
    }

    newMacro.pins[name] = newPin;
}

void readMacro(ifstream &read, string name){
    string buf;
    vector <string> Buf;
    getline(read, buf);
    Buf = Extract(buf);

    macro newMacro;
    newMacro.label = name;
    if(Buf.size() == 3 && Buf[0] == "CLASS" && Buf[2] == ";"){

        getline(read, buf);
        Buf = Extract(buf);

        while(!(Buf.size() == 2 && Buf[0] == "END" && Buf[1] == name)){

            if(Buf.size() == 3 && Buf[0] == "CLASS" && Buf[2] == ";"){
                newMacro.Class = Buf[1];
            }

            else if(Buf.size() == 5 && Buf[0] == "SIZE" && Buf[2] == "BY" && Buf[4] == ";"){
                newMacro.width = round(stod(Buf[1]) * 100);
                newMacro.height = round(stod(Buf[3]) * 100);
            }

            else if(Buf.size() == 2 && Buf[0] == "PIN"){
                readPin(read, Buf[1], newMacro);
            }
            else if(Buf.size() == 1 && Buf[0] == "OBS"){
                readObs(read, newMacro);
            }

            getline(read, buf);
            Buf = Extract(buf);
        }
    }

    macros[name] = newMacro;
}

void readVia(ifstream &read, string name, bool Default){


    string buf;
    vector <string> Buf;
    int layer_index = 0;

    via newVia;
    newVia.Default = Default;
    newVia.label = name;
    getline(read, buf);
    Buf = Extract(buf);
    while(!(Buf.size() == 2 && Buf[0] == "END" && Buf[1] == name)){
        if(Buf.size() == 3 && Buf[0] == "LAYER" && Buf[2] == ";"){
            if(layer_index == 0){
                newVia.botLayerIndex = metalTable[Buf[1]];
                getline(read, buf);
                Buf = Extract(buf);
                int index = 1;
                if(Buf.size() && Buf[0] == "RECT" || Buf[0] == "POLYGON"){
                    while(index < Buf.size() && Buf[index] != ";"){
                        newVia.botLayerShape.push_back(make_pair(stod(Buf[index]), stod(Buf[index + 1])));
                        index += 2;
                    }
                }
                layer_index++;
            }
            else if(layer_index == 1){
                newVia.cutLayerIndex = cutTable[Buf[1]];
                getline(read, buf);
                Buf = Extract(buf);
                int index = 1;
                if(Buf.size() && Buf[0] == "RECT" || Buf[0] == "POLYGON"){
                    while(index < Buf.size() && Buf[index] != ";"){
                        newVia.cutLayerShape.push_back(make_pair(stod(Buf[index]), stod(Buf[index + 1])));
                        index += 2;
                    }
                }
                layer_index++;
            }
            else if(layer_index == 2){
                newVia.topLayerIndex = metalTable[Buf[1]];
                getline(read, buf);
                Buf = Extract(buf);
                int index = 1;
                if(Buf.size() && Buf[0] == "RECT" || Buf[0] == "POLYGON"){
                    while(index < Buf.size() && Buf[index] != ";"){
                        newVia.topLayerShape.push_back(make_pair(stod(Buf[index]), stod(Buf[index + 1])));
                        index += 2;
                    }
                }
                layer_index++;
            }
        }
        getline(read, buf);
        Buf = Extract(buf);
    };
    vias.push_back(newVia);
}

void readLayer(ifstream &read, string name){
    string buf;
    vector <string> Buf;

    getline(read, buf);
    Buf = Extract(buf);
    if(Buf.size() == 3 && Buf[0] == "TYPE" && Buf[2] == ";"){
        if(Buf[1] == "ROUTING"){
            metal newMetal;
            getline(read, buf);
            Buf = Extract(buf);
            while(!(Buf.size() == 2 && Buf[0] == "END" && Buf[1] == name)){
                if(Buf.size() == 3 && Buf[2] == ";"){
                    if(Buf[0] == "DIRECTION") newMetal.dir = Buf[1][0];
                    else if(Buf[0] == "PITCH") round(newMetal.pitch = stod(Buf[1]) * 100);
                    else if(Buf[0] == "WIDTH") round(newMetal.width = stod(Buf[1]) * 100);
                    else if(Buf[0] == "SPACING") round(newMetal.spacing = stod(Buf[1]) * 100);
                    else if(Buf[0] == "OFFSET") round(newMetal.offset = stod(Buf[1]) * 100);
                }
                getline(read, buf);
                Buf = Extract(buf);
            }
            newMetal.label = name;
            newMetal.index = metalLayers.size();
            metalTable[name] = metalLayers.size();
            metalLayers.push_back(newMetal);
        }
        else if(Buf[1] == "CUT"){
            cut newCut;
            getline(read, buf);
            Buf = Extract(buf);
            while(!(Buf[0] == "END" && Buf[1] == name)){
                if(Buf.size() == 3 && Buf[2] == ";"){
                    if(Buf[0] == "SPACING") newCut.spacing = stod(Buf[1]);
                }
                getline(read, buf);
                Buf = Extract(buf);
            }
            newCut.label = name;
            newCut.index = cutLayers.size();
            cutTable[name] = cutLayers.size();
            cutLayers.push_back(newCut);

        }
    }


}


bool readLEF(){
    string source, buf;
	vector <string> Buf;
	cout << "Enter the path for the .lef file: ";
	getline(cin, source);
	ifstream read(source);
	if(!read.is_open()){
	  cout << "Error: Unable to read the .lef file.\n";
	  return 0;
	}
	int line = 1;
	getline(read, buf);
	while(!read.eof()){
		Buf = Extract(buf);
		int index = 0;
		while(index < Buf.size()){
			if(Buf[index] == "VERSION"){
				if(Buf.size() > index + 2 && Buf[index + 2] == ";"){
					LEF_FILE.ver = Buf[index + 1];
					index += 3;
				}
				else {
                    cout << "Undefined parameters list after \"VERSION\"\n";
                    return 0;
				}
			}

			else if(Buf[index] == "DIVIDERCHAR"){
				if(Buf.size() > index + 2 && Buf[index + 2] == ";" && Buf[index + 1].length() == 3 && Buf[index + 1][0] == '\"' && Buf[index + 1][2] == '\"'){
					LEF_FILE.div = Buf[index + 1][1];
					index += 3;
				}
				else {
                    cout << "Undefined parameters list after \"DIVIDERCHAR\"\n";
                    return 0;
				}
			}

            else if(Buf[index] == "BUSBITCHARS"){
                if(Buf.size() > index + 2 && Buf[index + 2] == ";" && Buf[index + 1].length() == 4 && Buf[index + 1][0] == '\"' && Buf[index + 1][3] == '\"'){
                    LEF_FILE.bus_left = Buf[index + 1][1];
                    LEF_FILE.bus_right = Buf[index + 1][2];
                    index += 3;
                }
                else {
                    cout << "Undefined parameters list after \"BUSBITCHARS\"\n";
                    return 0;
                }
            }

            else if(Buf[index] == "UNITS"){
                getline(read, buf);
                while(buf != "END UNITS"){
                    Buf = Extract(buf);
                    if(Buf.size() == 4 && Buf[0] == "DATABASE" && Buf[1] == "MICRONS" && Buf[3] == ";"){
                        LEF_FILE.unit = stoi(Buf[2]);
                    }
                    getline(read, buf);
                }
                index = Buf.size();
            }

            else if(Buf[index] == "LAYER"){
                if(Buf.size() > index + 1){
                    readLayer(read, Buf[1]);
                }
                else{
                    cout << "Undefined parameters list after \"LAYER\"\n";
                    return 0;
                }
                index = Buf.size();
            }

            else if(Buf[index] == "VIA"){
                if(Buf.size() > index + 1){
                    readVia(read, Buf[1], (Buf.size() > index + 2 && Buf[2] == "DEFAULT"));

                }
                else{
                    cout << "Undefined parameters list after \"VIA\"\n";
                    return 0;
                }
                index = Buf.size();
            }

            else if(Buf[index] == "MACRO"){
                if(Buf.size() > index + 1){
                    readMacro(read, Buf[1]);
                }
                else{
                    cout << "Undefined parameters list after \"MACRO\"\n";
                    return 0;
                }
                index = Buf.size();
            }

			else index = Buf.size();
		}
        getline(read, buf);
	}
	return 1;
}
/*
int main(){
    cout << readLEF() << endl;
    cout << readDEF() << endl;

    cout << nets.size() << endl;
    cout << order.size() << endl << endl << endl;
    for(int i =0; i < order.size(); i++)cout << order[i] << endl;

    /*
    cout << "\nComponents Section\n";
    for(auto i = components.begin(); i != components.end(); i++){
        component t = i->second;
        cout << t.instance << " from the cell " << t.macro << " . Positioned in " << t.x_pos << ", " << t.y_pos << " as " << t.orientation << endl;
    }

    cout << "\n\nNets Section\n";
    for(int i = 0; i < nets.size(); i++){
        cout << "NET #" << i + 1 << endl;
        for(int j = 0; j < nets[i].size(); j++){
            cout << "\tPORT #" << j + 1 << endl;
            for(int k = 0; k < nets[i][j].size(); k++){
                cout << "\t\tRECT @" << nets[i][j][k].metalLayer << " COO " << nets[i][j][k].x1 << " " << nets[i][j][k].y1 << " " << nets[i][j][k].x2 << " " << nets[i][j][k].y2 << endl;
            }
        }
    }
     cout << "\nObstructions Section\n";
    for(int i = 0; i < obs.size(); i++){
        cout << "In layer " << obs[i].metalLayer << " Rectangle " << obs[i].x1 << " " << obs[i].y1 << " " << obs[i].x2 << " " << obs[i].y2 << endl;
    }

    */
    /*
    cout << "LEF SECTION" << endl;
    cout << "CHARS    " << LEF_FILE.bus_left << "    " << LEF_FILE.bus_right << endl;
    cout << "Database units: " << LEF_FILE.unit << endl;

    cout << "\nMETALS SECTION" << endl;
       for(int i = 0; i < metalLayers.size(); i++){
        cout << "metal layer #" << metalLayers[i].index << endl;
        cout << "\tDirection:  " << metalLayers[i].dir << endl;
        cout << "\tOffset = " << metalLayers[i].offset << " , Width = " << metalLayers[i].width << endl;
        cout << "\tPitch = " << metalLayers[i].pitch << " , Spacing = " << metalLayers[i].spacing << endl;
        //cout << "Num of Tracks = " << metalLayers[i].numTracks << endl;
        cout << metalLayers[i].index << endl;
        cout << metalLayers[i].offset << endl;
        cout << metalLayers[i].pitch << endl;
        cout << metalLayers[i].spacing << endl;
        cout << metalLayers[i].width << endl;
        cout << "FROM THE DEF" << endl;
        cout << metalLayers[i].start << endl;
        cout << metalLayers[i].numTracks << endl;
        cout << metalLayers[i].step << endl;

    }


    cout << "\n\nMICROS SECTION" << endl;
    for(auto i = macros.begin(); i != macros.end(); i++){
        cout << "MACRO " << i->first << endl;
        macro ty = i->second;
        for(auto j = ty.pins.begin(); j != ty.pins.end(); j++){
            cout << "\tPIN " << j->first << endl;
            vector<RECT> vec = j->second.ports;
            for(int k = 0; k < vec.size(); k++){
                cout << "\t\tIn metal layer #" << vec[k].metalLayer << " RECT " << vec[k].x1 << " " << vec[k].y1 << " " << vec[k].x2 << " " << vec[k].y2 << endl;
            }
        }
        cout << "\tObstruction" << endl;
        for(int k = 0; k < ty.obs.size(); k++){
            cout << "\t\tIn metal layer #" << ty.obs[k].metalLayer << " RECT " << ty.obs[k].x1 << " " << ty.obs[k].y1 << " " << ty.obs[k].x2 << " " << ty.obs[k].y2 << endl;
        }

    }

    cout << "\n\nNets Section\n";
    cout << "NUM OF NETS " << nets.size() << endl;
    for(int i = 0; i < nets.size(); i++){
        cout << "\tNET #" << i + 1 << " has NUM OF PINS " << nets[i].size() << endl;
        for(int j = 0; j < nets[i].size(); j++){
            cout << "\t\tThis pin has NUM OF PORTS " << nets[i][j].size() << endl;
        }
    }
    */
}

/*
int main(){
    cout << readLEF() << endl;
    cout << readDEF() << endl;


    cout << "LEF SECTION" << endl;
    cout << "CHARS    " << LEF_FILE.bus_left << "    " << LEF_FILE.bus_right << endl;
    cout << LEF_FILE.database << endl;
    cout << LEF_FILE.div << endl;
    cout << LEF_FILE.ver << endl;
    cout << "METALS SECTION" << endl;
       for(int i = 0; i < metalLayers.size(); i++){
        cout << metalLayers[i].dir << endl;
        cout << metalLayers[i].index << endl;
        cout << metalLayers[i].offset << endl;
        cout << metalLayers[i].pitch << endl;
        cout << metalLayers[i].spacing << endl;
        cout << metalLayers[i].width << endl;
        cout << "FROM THE DEF" << endl;
        cout << metalLayers[i].start << endl;
        cout << metalLayers[i].numTracks << endl;
        cout << metalLayers[i].step << endl;

    }
    for(int i = 0; i < cutLayers.size(); i++){
        cout << cutLayers[i].index << endl;
        cout << cutLayers[i].spacing << endl;
    }
    cout << "TABLEES\n\n";
    for(auto i = metalTable.begin(); i != metalTable.end(); i++){
        cout << i->first << "   " << i->second << endl;
    }
    cout << "////////////////\n";
    for(auto i = cutTable.begin(); i != cutTable.end(); i++){
        cout << i->first << "   " << i->second << endl;
    }

    cout << "VIAS SECTION" << endl;
    for(int i = 0; i < vias.size(); i++){
        cout << "index   " << i << endl;
        cout << vias[i].botLayerIndex << endl;
        for(int j = 0; j < vias[i].botLayerShape.size(); j++)cout << vias[i].botLayerShape[j].first << ", " << vias[i].botLayerShape[j].second << "     ";
        cout << endl;
        cout << vias[i].cutLayerIndex << endl;
        for(int j = 0; j < vias[i].cutLayerShape.size(); j++)cout << vias[i].cutLayerShape[j].first << ", " << vias[i].cutLayerShape[j].second << "     ";
        cout << endl;
        cout << vias[i].topLayerIndex << endl;
        for(int j = 0; j < vias[i].topLayerShape.size(); j++)cout << vias[i].topLayerShape[j].first << ", " << vias[i].topLayerShape[j].second << "     ";
        cout << endl;
    }

    cout << "MICROS SECTION" << endl;
    for(auto i = macros.begin(); i != macros.end(); i++){
        cout << "MACRO " << i->first << endl;
        macro ty = i->second;
        for(auto j = ty.pins.begin(); j != ty.pins.end(); j++){
            cout << "\tPIN " << j->first << endl;
            vector<RECT> vec = j->second.ports;
            for(int k = 0; k < vec.size(); k++){
                cout << "\t\tIn layer " << vec[k].metalLayer << " RECT " << vec[k].x1 << " " << vec[k].y1 << " " << vec[k].x2 << " " << vec[k].y2 << endl;
            }
        }
        cout << "\tObstruction" << endl;
        for(int k = 0; k < ty.obs.size(); k++){
            cout << "\t\tIn layer " << vec[k].metalLayer << " RECT " << vec[k].x1 << " " << vec[k].y1 << " " << vec[k].x2 << " " << vec[k].y2 << endl;
        }

    }

    cout << "\nObstructions Section\n";
    for(int i = 0; i < obs.size(); i++){
        cout << "In layer " << obs[i].metalLayer << " Rectangle " << obs[i].x1 << " " << obs[i].y1 << " " << obs[i].x2 << " " << obs[i].y2 << endl;
    }

     cout << "\nComponents Section\n";
    for(auto i = components.begin(); i != components.end(); i++){
        component t = i->second;
        cout << t.instance << " from the cell " << t.macro << " . Positioned in " << t.x_pos << ", " << t.y_pos << " as " << t.orientation << endl;
    }

    cout << "\n\nNets Section\n";
    for(int i = 0; i < nets.size(); i++){
        cout << "NET #" << i + 1 << endl;
        for(int j = 0; j < nets[i].size(); j++){
            cout << "\tPORT #" << j + 1 << endl;
            for(int k = 0; k < nets[i][j].size(); k++){
                cout << "\t\tRECT @" << nets[i][j][k].metalLayer << " COO " << nets[i][j][k].x1 << " " << nets[i][j][k].y1 << " " << nets[i][j][k].x2 << " " << nets[i][j][k].y2 << endl;
            }
        }
    }


    */

//}
