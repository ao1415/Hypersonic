#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <stack>
#include <queue>

using namespace std;

const char Cell = '.';
const char EmptyBox = '0';
const char RangeBox = '1';
const char ExtraBox = '2';
const char Wall = 'X';

const int Player = 0;
const int Bomb = 1;
const int Item = 2;

const string CMove = "MOVE ";
const string CBomb = "BOMB ";

enum class Table {
	Cell,
	/// <summary>��</summary>
	EmptyBox,
	/// <summary>�����A�b�v�A�C�e������</summary>
	RangeBox,
	/// <summary>�{���ǉ��A�C�e������</summary>
	ExtraBox,
	/// <summary>��</summary>
	Wall,
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

	Entitie() : Entitie(Point(), 0, 0) {}
	Entitie(const Point& p, const int v1, const int v2) {
		point = p; val1 = v1; val2 = v2;
	}

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
	static const vector<Entitie> MyB() { return myB; }
	static const Entitie MyB(const size_t n) { return myB[n]; }
	static const vector<Entitie> EnB() { return enB; }
	static const Entitie EnB(const size_t n) { return enB[n]; }
	static const vector<Entitie> Item() { return item; }
	static const Entitie Item(const size_t n) { return item[n]; }

private:

	static int width;
	static int height;
	static int myId;

	static StageArray stage;
	static Entitie my;
	static Entitie en;
	static vector<Entitie> myB;
	static vector<Entitie> enB;

	static vector<Entitie> item;

	static void EntitieReset() {
		my = Entitie();
		en = Entitie();
		myB.clear();
		enB.clear();
		item.clear();
	}

};

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
				switch (c)
				{
				case Cell:
					Share::stage[x][y] = Table::Cell;
					break;
				case EmptyBox:
					Share::stage[x][y] = Table::EmptyBox;
					break;
				case RangeBox:
					Share::stage[x][y] = Table::RangeBox;
					break;
				case ExtraBox:
					Share::stage[x][y] = Table::ExtraBox;
					break;
				case Wall:
					Share::stage[x][y] = Table::Wall;
					break;
				default:
					Share::stage[x][y] = Table::Cell;
					break;
				}
			}
			cin.ignore();
		}
		int entities;
		cin >> entities; cin.ignore();

		Share::EntitieReset();

		for (int i = 0; i < entities; i++) {
			int entityType;
			int owner;
			int x;
			int y;
			int param1;
			int param2;
			cin >> entityType >> owner >> x >> y >> param1 >> param2;
			cin.ignore();

			if (entityType == Player)
			{
				if (owner == Share::myId)
				{
					Share::my = Entitie(Point(x, y), param1, param2);
					cerr << param1 << endl;
				}
				else
					Share::en = Entitie(Point(x, y), param1, param2);
			}
			else if (entityType == Bomb)
			{
				if (owner == Share::myId)
					Share::myB.push_back(Entitie(Point(x, y), param1, param2));
				else
					Share::enB.push_back(Entitie(Point(x, y), param1, param2));
			}
			else if (entityType == Item)
			{
				Share::item.push_back(Entitie(Point(x, y), param1, param2));
			}
		}

	}

};

int Share::width;
int Share::height;
int Share::myId;

StageArray Share::stage;
Entitie Share::my;
Entitie Share::en;
vector<Entitie> Share::myB;
vector<Entitie> Share::enB;
vector<Entitie> Share::item;

const Point Direction[] = { Point(0,-1),Point(-1,0),Point(1,0),Point(0,1) };

const bool inside(const Point& p) { return (0 <= p.x && 0 <= p.y && p.x < Share::width && p.y < Share::height); }
const int range(const Point& p1, const Point& p2) { return (abs(p1.x - p2.x) + abs(p1.y - p2.y)); }
const bool isBox(const Table& t) { return (t == Table::EmptyBox || t == Table::RangeBox || t == Table::ExtraBox); }

class AI {
public:

	const string think() {
		string command = CMove + Point(Share::Width() - 1, Share::Height() - 1).toString();


		cerr << "���W:" << Share::My().point.toString() << endl;
		if (!destination)
		{
			cerr << "�ړI�Ȃ�" << endl;
			destination = search();
			command = CMove + destination.toString();
			cerr << command << endl;
		}

		cerr << "�ړI����" << endl;
		if (destination == Share::My().point)
		{
			command = CBomb + destination.toString();
			if (Share::My().val1 > 0)
			{
				destination = Point();
			}
			cerr << command << endl;
		}
		else
		{
			command = CMove + destination.toString();
			cerr << command << endl;
		}

		return command;
	}

private:

	/// <summary>�ړI�n</summary>
	Point destination;

	const Point search(const int r = 8) const {

		const Point myPoint = Share::My().point;

		Point maxPoint = myPoint;
		int maxBox = 0;
		int maxRange = 0;

		const int CheckArrayDefault = -1;
		vector<vector<int>> check;
		check.resize(Share::Width());
		for (auto& c : check) c.resize(Share::Height(), CheckArrayDefault);

		queue<Point> que;
		que.push(myPoint);
		check[myPoint.x][myPoint.y] = 0;

		while (!que.empty())
		{
			const auto point = que.front();
			que.pop();
			const int rng = check[point.x][point.y];

			const int box = destroyBox(point);

			if (box > maxBox)
			{
				maxBox = box;
				maxRange = rng;
				maxPoint = point;
			}
			else if (box == maxBox && rng > maxRange)
			{
				maxRange = rng;
				maxPoint = point;
			}

			if (rng <= r)
			{
				for (const auto& dire : Direction)
				{
					const auto next = point + dire;
					if (inside(next) && check[next.x][next.y] == CheckArrayDefault && Share::Stage(next) == Table::Cell)
					{
						que.push(next);
						check[next.x][next.y] = rng + 1;
					}
				}
			}
		}
		/*
		//�v���C���[���s���ł���ꏊ
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
		*/

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
				if (isBox(Share::Stage(nextPoint)))
					boxCount++;
			}
		}
		for (int dx = -r + 1; dx < r; dx++)
		{
			const Point nextPoint = point + Point(dx, 0);
			if (inside(nextPoint))
			{
				if (isBox(Share::Stage(nextPoint)))
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