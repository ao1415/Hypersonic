#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

const char Cell = '.';
const char Box = '0';

const int Player = 0;
const int Bomb = 1;

const string CMove = "MOVE ";
const string CBomb = "BOMB ";

enum class Stage {
	None,
	Box,
	Size
};

typedef vector<vector<Stage>> StageArray;

struct Point {
	int x;
	int y;

	Point() :Point(0, 0) {}
	Point(const int _x, const int _y) { x = _x; y = _y; }

	const bool operator==(const Point& p) const { return (x == p.x && y == p.y); }
	const bool operator!=(const Point& p) const { return !(*this == p); }

	const string toString() const { return to_string(x) + " " + to_string(y); }

};

struct Entitie {

	Entitie() : Entitie(false, Point(), 0, 0) {}
	Entitie(const bool f, const Point& p, const int v1, const int v2) {
		flag = f; point = p; val1 = v1; val2 = v2;
	}

	bool flag;
	Point point;
	int val1;
	int val2;
};

struct Input;
const bool inside(const Point& p);

class Share {
public:

	friend Input;
	friend const bool inside(const Point& p);

	static const int Width() { return width; }
	static const int Height() { return height; }
	static const int MyId() { return myId; }

	static const StageArray Stage() { return stage; }
	static const Entitie My() { return my; }
	static const Entitie En() { return en; }
	static const Entitie MyB() { return myB; }
	static const Entitie EnB() { return enB; }

private:

	static int width;
	static int height;
	static int myId;

	static StageArray stage;
	static Entitie my;
	static Entitie en;
	static Entitie myB;
	static Entitie enB;

	static void EntitieReset() {
		my = Entitie();
		en = Entitie();
		myB = Entitie();
		enB = Entitie();
	}

};

int Share::width;
int Share::height;
int Share::myId;

StageArray Share::stage;
Entitie Share::my;
Entitie Share::en;
Entitie Share::myB;
Entitie Share::enB;

struct Input {

	static void first() {
		cin >> Share::width >> Share::height >> Share::myId;
		cin.ignore();

		Share::stage.resize(Share::width);
		for (auto& s : Share::stage) s.resize(Share::height);
	}

	static void loop() {

		for (int y = 0; y < Share::Height(); y++)
		{
			for (int x = 0; x < Share::Width(); x++)
			{
				char c;
				cin >> c;
				Share::stage[x][y] = (c == Cell) ? Stage::None : Stage::Box;
			}
			cin.ignore();
		}
		int entities;
		cin >> entities; cin.ignore();
		for (int i = 0; i < entities; i++) {
			int entityType;
			int owner;
			int x;
			int y;
			int param1;
			int param2;
			cin >> entityType >> owner >> x >> y >> param1 >> param2;
			cin.ignore();

			Share::EntitieReset();
			if (entityType == Player)
			{
				if (owner == Share::myId)
					Share::my = Entitie(true, Point(x, y), param1, param2);
				else
					Share::en = Entitie(true, Point(x, y), param1, param2);
			}
			else
			{
				if (owner == Share::myId)
					Share::myB = Entitie(true, Point(x, y), param1, param2);
				else
					Share::enB = Entitie(true, Point(x, y), param1, param2);
			}

		}

	}

};

const bool inside(const Point& p) { return (0 <= p.x && 0 <= p.y && p.x < Share::width && p.y < Share::height); }
const int range(const Point& p1, const Point& p2) { return (abs(p1.x - p2.x) + abs(p1.y - p2.y)); }

class AI {
public:

	const string think() {
		string command = CMove + Point(0, 0).toString();
		command = CBomb + Point(10, 12).toString();
		return command;
	}

private:


};

int main()
{
	Input::first();

	AI ai;

	while (true)
	{
		Input::loop();



		cout << ai.think() << endl;
	}
}
