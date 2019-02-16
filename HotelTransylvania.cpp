#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#define GUESTFILE "guest.txt"
#define ROOMFILE "rooms.txt"

#define serviceRate 10000

using namespace std;

struct Guest{
  string name;
  string number; 
};

struct Room{
	int id;
	string type;
	int capacity;
	int rate;
	bool reserved;
};

struct Reserve{
	Guest guest;
	Room room;
	int days;
	int serviceNum;
};

void openFile(string fileName , ifstream &fileData);
void readGuestsData(ifstream &fileData , vector <Guest> &guests);
void readGuestDetails(string line , Guest &guest);
void readRoomsData(ifstream &fileData , vector <Room> &rooms);
void readRoomDetails(string line , Room &room);
vector<string> getCommand();
void runCommand(vector<string> commandParts, vector<Guest> &guests, vector<Room> &rooms,
	vector<Reserve> &reserves);
vector<Room> getStatus(vector<Room> rooms, string type, string value);
vector<Room> getStatusByID(vector<Room> rooms, string id);
vector<Room> getStatusByType(vector<Room> rooms, string type);
void showRooms(vector<Room> rooms);
void reserve(vector<Reserve> &reserves,vector<Guest> guests,vector<Room> &rooms,
	string id, string guestName, string days);
void service(vector<Reserve> &reserves, string roomID, string guestName);
void checkout(vector<Reserve> &reserves, string roomID, string guestName);
Reserve findReserve(vector<Reserve> &reserves, string guestName, int roomID, bool del);
Guest findGuestByName(vector<Guest> guests, string name);
Room findRoomByID(vector<Room> &rooms, int id, bool reserving);
vector<string> splitBySpace(string statement);

int main(){
	vector<Reserve> reserves;
	vector <Guest> guests;
	ifstream guestsFile;
	openFile(GUESTFILE , guestsFile);
	if(!guestsFile){
		return 0;
	}
	readGuestsData(guestsFile , guests);	
	vector <Room> rooms;
	ifstream roomsFile;
	openFile(ROOMFILE , roomsFile);
	if(!roomsFile){
		return 0;
	}
	readRoomsData(roomsFile , rooms);
	roomsFile.close();

	while (cin != NULL){
		vector <string> commandParts = getCommand();
		runCommand(commandParts , guests , rooms, reserves);
		commandParts.clear();
	}

	return 0;
}

void openFile(string fileName , ifstream &fileData){
  fileData.open(fileName.c_str());
  if (!fileData){
    cerr << "can't open file [" << fileName << "]\n";
  }
}

void readGuestsData(ifstream &fileData , vector <Guest> &guests){
  string line;
  Guest guest;
  while (fileData.tellg()!=EOF){
    getline(fileData, line);
    readGuestDetails(line , guest);
    guests.push_back(guest);
  }
}

void readGuestDetails(string line , Guest &guest){
 	vector<string> details = splitBySpace(line);
 	guest.name = details[0];
 	guest.number = details[1];
}

void readRoomsData(ifstream &fileData , vector <Room> &rooms){
  string line;
  Room room;
  while (fileData.tellg()!=EOF){
    getline(fileData, line);
    readRoomDetails(line , room);
    rooms.push_back(room);
  }
}

void readRoomDetails(string line , Room &room){
 	vector<string> details = splitBySpace(line);
 	room.id = stoi(details[0]);
 	room.type = details[1];
 	room.capacity = stoi(details[2]);
 	room.rate = stoi(details[3]);
 	room.reserved = false;
}

vector<string> getCommand(){
	string line;
	getline (cin , line);
	vector<string> commandParts = splitBySpace(line);
	return commandParts;
}

void runCommand(vector<string> commandParts,vector<Guest> &guests,vector<Room> &rooms,
	vector<Reserve> &reserves){
	if(commandParts[0] == "status"){
		vector<Room> roomsToShow = getStatus(rooms, commandParts[1], commandParts[2]);
		showRooms(roomsToShow);
	}
	else if(commandParts[0] == "listAllRooms")
		showRooms(rooms);
	else if(commandParts[0] == "reserve")
		reserve(reserves, guests, rooms, commandParts[1], commandParts[2], commandParts[3]);
	else if(commandParts[0] == "service")
		service(reserves, commandParts[1],commandParts[2]);
	else if(commandParts[0] == "checkout")
		checkout(reserves, commandParts[1],commandParts[2]);
	else
		cout << "Invalid command!" << endl;
}

vector<Room> getStatus(vector<Room> rooms, string type, string value){
	vector <Room> roomsToShow;
	if(type == "id")
		roomsToShow = getStatusByID(rooms, value);
	else if(type == "type")
		roomsToShow = getStatusByType(rooms, value);
	else
		cout << "Command is invalid! Type should be id or type." << endl;
	return roomsToShow;
}

vector<Room> getStatusByID(vector<Room> rooms, string id){
	vector<Room> roomsToShow;
	roomsToShow.push_back(findRoomByID(rooms, stoi(id), false));
	return roomsToShow;
}

vector<Room> getStatusByType(vector<Room> rooms, string type){
	vector<Room> roomsToShow;
	for (int i = 0; i < rooms.size(); i++){
		if (rooms[i].type == type)
			roomsToShow.push_back(rooms[i]);
	}
	return roomsToShow;
}

void showRooms(vector<Room> rooms){
	if (rooms.size() != 0){
		cout << "Room id\tRoom type\tRoom capacity\tRoom rate\tRoom status" << endl;
		cout << "_________________________________________________________" << endl;
		for (int i = 0; i < rooms.size(); i++){
			cout << rooms[i].id << "\t" << rooms[i].type << "\t" << rooms[i].capacity << "\t" << 
				rooms[i].rate << "\t";
			if (rooms[i].reserved == false)
				cout << "empty :)" << endl;
			else
				cout << "reserved :(" << endl;  	
		}
	}else
		cout<< "No rooms with this details found!" << endl;
}

void reserve(vector<Reserve> &reserves,vector<Guest> guests,vector<Room> &rooms,
	string id, string guestName, string days){
	Reserve newReserve;
	Guest guest = findGuestByName(guests, guestName);
	Room room = findRoomByID(rooms, stoi(id) , true);
	if (room.reserved == false){
		newReserve.guest = guest;
		newReserve.room = room;
		newReserve.days = stoi(days);
		newReserve.serviceNum = 0;
		reserves.push_back(newReserve);
	}else
		cout << "Room already is reserved." << endl;
}

void service(vector<Reserve> &reserves, string roomID, string guestName){
	for (int i = 0; i < reserves.size(); i++){
		if (guestName == reserves[i].guest.name && stoi(roomID) == reserves[i].room.id)//delete stoi
			reserves[i].serviceNum += 1;
	}
}

void checkout(vector<Reserve> &reserves, string roomID, string guestName){
	Reserve reserve = findReserve(reserves, guestName, stoi(roomID) , true);
	cout << guestName << " " << roomID << " " << reserve.days << endl;
	int roomPrice = reserve.days * reserve.room.rate;
	cout << "Room price:" << reserve.days << " * " << reserve.room.rate << " = " << roomPrice << endl; 
	int servicePrice = reserve.serviceNum * serviceRate;
	cout << "Service prive:" << reserve.serviceNum << " * " << serviceRate << " = " << servicePrice << endl;
	int priceToPay = roomPrice + servicePrice;
	cout << "Price To pay = " << priceToPay << endl;
}

Reserve findReserve(vector<Reserve> &reserves, string guestName, int roomID, bool del){
	for (int i = 0; i < reserves.size(); i++){
		if (guestName == reserves[i].guest.name && roomID == reserves[i].room.id){
			Reserve temp = reserves[i];
			if (del == true)
				reserves.erase(reserves.begin() + i);
			return temp;
		}
	}
}

Guest findGuestByName(vector<Guest> guests, string name){
	for (int i = 0; i < guests.size(); i++){
		if (name == guests[i].name)
			return guests[i];
	}
	cout << "Guest not found!" << endl;
}

Room findRoomByID(vector<Room> &rooms, int id, bool reserving){
	for (int i = 0; i < rooms.size(); i++){
		if (rooms[i].id == id){
			Room temp = rooms[i];
			if (reserving == true)
				rooms[i].reserved = true;
			return temp;
		}
	}
}

vector<string> splitBySpace(string statement){
    vector<string> result;
    string temp = "";
    string token;
    for(int i=0; i<statement.length(); i++)
        if(statement[i] != ' '){
        	token += statement[i];
        }
        else if(token.length()) {
            result.push_back(token);
            token = "";
        }
				result.push_back(token);
    return result;
}