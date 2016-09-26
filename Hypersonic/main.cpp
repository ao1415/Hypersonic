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

enum class Table {
	Cell,
	Box,
	Size
};

typedef vector<vector<Table>> StageArray;

struct Point {
	int x;
	int y;

	Point() :Point(-1, -1) {}
	Point(const int _x, const int _y) { x = _x; y = _y; }

	const Point operator+(const Point& p) const { return Point(x + p.x, y + p.y); }
	const Point operator-(const Point& p) const { return Point(x - p.x, y - p.y); }

	const bool operator==(const Point& p) const { return (x == p.x && y == p.y); }
	const bool operator!=(const Point& p) const { return !(*this == p); }

	const string toString() const { return to_string(x) + " " + to_string(y); }

	operator bool() const { return !(x == -1 && y == -1); }

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
	static const Table Stage(const Point& p) { return stage[p.x][p.y]; }
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
				Share::stage[x][y] = ((c == Cell) ? Table::Cell : Table::Box);
			}
			cin.ignore();
		}
		int entities;
		cin >> entities; cin.ignore();

		Share::EntitieReset();

		//cerr << "Entitie" << endl;

		for (int i = 0; i < entities; i++) {
			int entityType;
			int owner;
			int x;
			int y;
			int param1;
			int param2;
			cin >> entityType >> owner >> x >> y >> param1 >> param2;
			cin.ignore();

			//cerr << entityType << "," << owner << "," << x << "," << y << "," << param1 << "," << param2 << endl;

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
		string command = CMove + Point(Share::Width() - 1, Share::Height() - 1).toString();


		cerr << "座標:" << Share::My().point.toString() << endl;
		if (!destination)
		{
			cerr << "目的なし" << endl;
			destination = search();
			command = CMove + destination.toString();
			cerr << command << endl;
		}
		else
		{
			cerr << "目的あり" << endl;
			if (destination == Share::My().point)
			{
				command = CBomb + destination.toString();
				destination = Point();
				cerr << command << endl;
			}
			else
			{
				command = CMove + destination.toString();
				cerr << command << endl;
			}
		}

		return command;
	}

private:

	/// <summary>目的地</summary>
	Point destination;

	const Point search(const int r = 8) const {

		const Point myPoint = Share::My().point;

		Point maxPoint = myPoint;
		int maxBox = 0;
		int maxRange = 0;

		//プレイヤーが行動できる場所
		for (int dy = -r; dy <= r; dy++)
		{
			for (int dx = -r; dx <= r; dx++)
			{
				const Point nextPoint = myPoint + Point(dx, dy);
				if (inside(nextPoint) && range(myPoint, nextPoint) <= r && Share::Stage(nextPoint) == Table::Cell)
				{
					const int box = destroyBox(nextPoint);
					const int rng = range(myPoint, nextPoint);

					if (box > maxBox)
					{
						maxBox = box;
						maxRange = rng;
						maxPoint = nextPoint;
					}
					else if (box == maxBox && rng > maxRange)
					{
						maxRange = rng;
						maxPoint = nextPoint;
					}
				}
			}
		}

		cerr << "r    :" << r << endl;
		cerr << "box  :" << maxBox << endl;
		cerr << "range:" << maxRange << endl;
		cerr << "point:" << maxPoint.toString() << endl;

		return maxPoint;
	}

	const int destroyBox(const Point& point) const {

		const int r = Share::My().val2;
		int boxCount = 0;

		for (int dy = -r + 1; dy < r; dy++)
		{
			const Point nextPoint = point + Point(0, dy);
			if (inside(nextPoint))
			{
				if (Share::Stage(nextPoint) == Table::Box)
					boxCount++;
			}
		}
		for (int dx = -r + 1; dx < r; dx++)
		{
			const Point nextPoint = point + Point(dx, 0);
			if (inside(nextPoint))
			{
				if (Share::Stage(nextPoint) == Table::Box)
					boxCount++;
			}
		}

		return boxCount;
	}

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
