#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <set>
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

const int ItemRange = 1;
const int ItemExtra = 2;

const string CMove = "MOVE ";
const string CBomb = "BOMB ";

enum class Table {
	Cell,
	/// <summary>空</summary>
	EmptyBox,
	/// <summary>爆風アップアイテム入り</summary>
	RangeBox,
	/// <summary>ボム追加アイテム入り</summary>
	ExtraBox,
	/// <summary>壁</summary>
	Wall,
	Size
};

typedef vector<vector<Table>> StageArray;

struct Size {
	int w;
	int h;

	Size() : Size(0, 0) {}
	Size(int _w, int _h) { w = _w; h = _h; }

};

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

	const bool operator<(const Point& p) const { return (y * 128 + x < p.y * 128 + p.x); }

};

template<typename Type>
class Grid {
public:

	Grid() : Grid(Size(0, 0)) {}
	Grid(int w, int h) : Grid(Size(w, h)) {}
	Grid(const Size& s) {
		this->s = s;
		resize(s);
	}
	Grid(int w, int h, Type v) : Grid(Size(w, h), v) {}
	Grid(const Size& s, const Type v) {
		this->s = s;
		resize(s, v);
	}

	const Type& operator[](const Point& p) const { return grid[p.y][p.x]; }
	Type& operator[](const Point& p) { return grid[p.y][p.x]; }
	const std::vector<Type>& operator[](int y) const { return grid[y]; }
	std::vector<Type>& operator[](int y) { return grid[y]; }

	const Size& size() const { return s; }
	void resize(int w, int h) { resize(Size(w, h)); }
	void resize(const Size& s) {
		grid.resize(s.h);
		for (auto& g : grid) g.resize(s.w);
	}
	void resize(int w, int h, const Type v) { resize(Size(w, h), v); }
	void resize(const Size& s, const Type v) {
		grid.resize(s.h);
		for (auto& g : grid) g.resize(s.w, v);
	}

	const std::string toString(const char c = ',') const {

		std::string str;

		for (size_t y = 0; y < grid.size(); y++)
		{
			for (size_t x = 0; x < grid[y].size(); x++)
			{
				str += std::to_string(grid[y][x]);
				str += c;
			}
			str += "\n";
		}

		return str;
	}

private:

	std::vector<std::vector<Type>> grid;
	Size s;

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

typedef vector<Entitie> Entities;

struct Input;
const bool inside(const Point& p);

class Share {
public:

	friend Input;
	friend const bool inside(const Point& p);

	inline static const int Width() { return width; }
	inline static const int Height() { return height; }
	inline static const int MyId() { return myId; }

	inline static const Grid<Table>& Stage() { return stage; }
	inline static const Table& Stage(const Point& p) { return stage[p]; }

	inline static const vector<vector<bool>>& BlockStage() { return blockStage; }
	inline static const bool BlockStage(const Point& p) { return blockStage[p.x][p.y]; }

	inline static const vector<vector<int>>& ItemStage() { return itemStage; }
	inline static const int ItemStage(const Point& p) { return itemStage[p.x][p.y]; }

	inline static const Entitie& My() { return my; }
	inline static const Entities& En() { return en; }
	inline static const Entitie& En(const size_t n) { return en[n]; }

	inline static const Entities& MyB() { return myB; }
	inline static const Entitie& MyB(const size_t n) { return myB[n]; }

	inline static const Entities& EnB() { return enB; }
	inline static const Entitie& EnB(const size_t n) { return enB[n]; }

	inline static const Entities& Item() { return item; }
	inline static const Entitie& Item(const size_t n) { return item[n]; }

private:

	static int width;
	static int height;
	static int myId;

	static Grid<Table> stage;
	static vector<vector<bool>> blockStage;
	static vector<vector<int>> itemStage;
	static Entitie my;
	static Entities en;
	static Entities myB;
	static Entities enB;

	static Entities item;

	static void EntitieReset() {
		my = Entitie();
		en.clear();
		myB.clear();
		enB.clear();
		item.clear();
		itemStage.clear();
		itemStage.resize(width);
		for (auto& s : itemStage) s.resize(height);
	}

};

int Share::width;
int Share::height;
int Share::myId;

Grid<Table> Share::stage;
vector<vector<bool>> Share::blockStage;
vector<vector<int>> Share::itemStage;
Entitie Share::my;
Entities Share::en;
Entities Share::myB;
Entities Share::enB;
Entities Share::item;

struct Input {

	static void first() {
		cin >> Share::width >> Share::height >> Share::myId;
		cin.ignore();

		Share::stage.resize(Share::width, Share::height);

	}

	static void loop() {

		Share::blockStage.clear();
		Share::blockStage.resize(Share::width);
		for (auto& s : Share::blockStage) s.resize(Share::height);

		for (int y = 0; y < Share::Height(); y++)
		{
			for (int x = 0; x < Share::Width(); x++)
			{
				char c;
				cin >> c;
				switch (c)
				{
				case Cell:
					Share::stage[Point(x, y)] = Table::Cell;
					Share::blockStage[x][y] = false;
					break;
				case EmptyBox:
					Share::stage[Point(x, y)] = Table::EmptyBox;
					Share::blockStage[x][y] = true;
					break;
				case RangeBox:
					Share::stage[Point(x, y)] = Table::RangeBox;
					Share::blockStage[x][y] = true;
					break;
				case ExtraBox:
					Share::stage[Point(x, y)] = Table::ExtraBox;
					Share::blockStage[x][y] = true;
					break;
				case Wall:
					Share::stage[Point(x, y)] = Table::Wall;
					Share::blockStage[x][y] = true;
					break;
				default:
					Share::stage[Point(x, y)] = Table::Cell;
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
				}
				else
					Share::en.push_back(Entitie(Point(x, y), param1, param2));
			}
			else if (entityType == Bomb)
			{
				if (owner == Share::myId)
				{
					Share::myB.push_back(Entitie(Point(x, y), param1, param2));
					Share::blockStage[x][y] = true;
				}
				else
				{
					Share::enB.push_back(Entitie(Point(x, y), param1, param2));
					Share::blockStage[x][y] = true;
				}
			}
			else if (entityType == Item)
			{
				Share::item.push_back(Entitie(Point(x, y), param1, param2));
				Share::blockStage[x][y] = true;
				Share::itemStage[x][y] = param1;
			}
		}

	}

};

const Point Direction[] = { Point(0,-1),Point(-1,0),Point(1,0),Point(0,1) };

template <class CharType>
inline std::basic_ostream<CharType>& operator << (std::basic_ostream<CharType>& os, const Size& v) { return os << CharType('(') << (int)v.w << CharType(',') << (int)v.h << CharType(')'); }
template <class CharType>
inline std::basic_ostream<CharType>& operator << (std::basic_ostream<CharType>& os, const Point& v) { return os << CharType('(') << (int)v.x << CharType(',') << (int)v.y << CharType(')'); }

const bool inside(const Point& p) { return (0 <= p.x && 0 <= p.y && p.x < Share::width && p.y < Share::height); }
const int range(const Point& p1, const Point& p2) { return (abs(p1.x - p2.x) + abs(p1.y - p2.y)); }
const bool isBox(const Table& t) { return (t == Table::EmptyBox || t == Table::RangeBox || t == Table::ExtraBox); }

class BombSimulator {
public:

	void simulate(const Entities myB = Share::MyB(), const Entities enB = Share::EnB(), const Entities item = Share::Item(), Grid<Table> stage = Share::Stage()) {

		Grid<pair<int, int>> bombStage(Share::Width(), Share::Height(), { 0,0 });
		Grid<int> itemStage(bombStage.size(), 0);

		for (const auto& b : myB) bombStage[b.point] = { b.val1,b.val2 };
		for (const auto& b : enB) bombStage[b.point] = { b.val1,b.val2 };

		for (const auto& i : item) itemStage[i.point] = i.val1;

		const int BombTimer = 8;
		dangers.resize(8);

		for (int i = 0; i < BombTimer; i++)
		{
			set<Point> danger;

			for (int y = 0; y < Share::Height(); y++)
			{
				for (int x = 0; x < Share::Width(); x++)
				{
					if (bombStage[y][x].first == 1)
					{
						auto d = destroy(Point(x, y), bombStage, itemStage, stage);
						for (const auto b : d) danger.insert(b);
					}
					bombStage[y][x].first--;
				}
			}

			for (const auto& p : danger)
			{
				if (itemStage[p] > 0)
					itemStage[p] = 0;

				if (stage[p] == Table::ExtraBox)
				{
					stage[p] = Table::Cell;
					itemStage[p] = ItemExtra;
				}
				else if (stage[p] == Table::RangeBox)
				{
					stage[p] = Table::Cell;
					itemStage[p] = ItemRange;
				}
				else if (stage[p] == Table::EmptyBox)
					stage[p] = Table::Cell;
			}

			dangers[i] = danger;

		}

	}

	const bool check(const Point& point) const {
		for (size_t i = 0; i < dangers.size(); i++)
			if (dangers[i].find(point) != dangers[i].end())
				return true;
		return false;
	}


private:

	vector<set<Point>> dangers;

	const set<Point> destroy(const Point& point, Grid<pair<int, int>>& bomb, Grid<int>& item, Grid<Table>& stage) {

		const int r = bomb[point].second;
		set<Point> danger;
		danger.insert(point);
		bomb[point] = { 0,0 };

		const auto insert = [&](const Point& p) {
			if (inside(p))
			{
				danger.insert(p);
				if (bomb[p].first > 0)
				{
					const auto d = destroy(p, bomb, item, stage);
					for (const auto& b : d) danger.insert(b);
				}
				else if (item[p] != 0 || stage[p] != Table::Cell)
					return true;
			}
			return false;
		};

		for (int dy = 1; dy < r; dy++)
		{
			const Point nextPoint = point + Point(0, dy);
			if (insert(nextPoint)) break;
		}
		for (int dy = 1; dy < r; dy++)
		{
			const Point nextPoint = point + Point(0, -dy);
			if (insert(nextPoint)) break;
		}

		for (int dx = 1; dx < r; dx++)
		{
			const Point nextPoint = point + Point(dx, 0);
			if (insert(nextPoint)) break;
		}
		for (int dx = 1; dx < r; dx++)
		{
			const Point nextPoint = point + Point(-dx, 0);
			if (insert(nextPoint)) break;
		}

		return danger;
	}

};

class Search {
public:

	const Point bocSearch(const int r = 8) const {

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

		cerr << "P:" << maxPoint.toString() << endl;
		cerr << "B:" << maxBox << endl;

		return maxPoint;
	}

	const int destroyBox(const Point& point) const {

		const int r = Share::My().val2;
		int boxCount = 0;

		for (int dy = 1; dy < r; dy++)
		{
			const Point nextPoint = point + Point(0, dy);
			if (inside(nextPoint) && Share::BlockStage(nextPoint) == true)
			{
				if (isBox(Share::Stage(nextPoint)))
					boxCount++;
				break;
			}
		}
		for (int dy = 1; dy < r; dy++)
		{
			const Point nextPoint = point + Point(0, -dy);
			if (inside(nextPoint) && Share::BlockStage(nextPoint) == true)
			{
				if (isBox(Share::Stage(nextPoint)))
					boxCount++;
				break;
			}
		}

		for (int dx = 1; dx < r; dx++)
		{
			const Point nextPoint = point + Point(dx, 0);
			if (inside(nextPoint) && Share::BlockStage(nextPoint) == true)
			{
				if (isBox(Share::Stage(nextPoint)))
					boxCount++;
				break;
			}
		}
		for (int dx = 1; dx < r; dx++)
		{
			const Point nextPoint = point + Point(-dx, 0);
			if (inside(nextPoint) && Share::BlockStage(nextPoint) == true)
			{
				if (isBox(Share::Stage(nextPoint)))
					boxCount++;
				break;
			}
		}

		return boxCount;
	}

	const int minItemRange() const {

		const auto my = Share::My();
		const auto items = Share::Item();

		const int ExtraPriority = 2;
		const int RangePriority = 1;

		const int RangeMax = 8;
		const int ExtraMax = 8;

		int itemRange = INT32_MAX;

		for (const auto& item : items)
		{
			if (item.val1 == ItemRange)
			{
				if (my.val2 <= RangeMax)
				{
					itemRange = min(itemRange, range(my.point, item.point));
				}
			}
			else if (item.val1 == ItemExtra)
			{
				if (my.val2 <= ExtraMax)
				{
					itemRange = min(itemRange, range(my.point, item.point));
				}
			}
		}
		return itemRange;
	}

	const Point itemSearch(const int r = 5) const {

		const Point myPoint = Share::My().point;

		Point itemPoint;
		int minRange = INT32_MAX;

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

			if (Share::ItemStage(point) == ItemExtra)
			{
				if (minRange > rng)
				{
					minRange = rng;
					itemPoint = point;
				}
			}
			else if (Share::ItemStage(point) == ItemRange)
			{
				if (minRange > rng)
				{
					minRange = rng;
					itemPoint = point;
				}
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

		cerr << "P:" << itemPoint.toString() << endl;
		cerr << "R:" << minRange << endl;

		return itemPoint;
	}


private:



};

class AI {
public:

	const string think() {
		string command = CMove + Point(Share::Width() - 1, Share::Height() - 1).toString();

		bombSimulator.simulate();

		bool bombFlag = false;

		if (!destination)
		{
			destination = search.bocSearch();
			command = CMove + destination.toString();
		}

		if (destination == Share::My().point)
		{
			if (search.destroyBox(destination) > 0)
			{
				command = CBomb + destination.toString();
				bombFlag = true;
				if (Share::My().val1 > 0)
				{
					destination = Point();
				}
			}
			else
			{
				if (subDestinationFlag)
				{
					destination = subDestination;
					subDestinationFlag = false;
					command = CMove + destination.toString();
				}
				else
				{
					destination = Point();
				}
			}
		}
		else
		{

			if (search.minItemRange() <= 5)
			{
				const Point d = search.itemSearch();
				if (d)
				{
					subDestination = destination;
					subDestinationFlag = true;
					destination = d;
				}
			}

			command = CMove + destination.toString();
		}

		updateRangeTable();
		Point movePoint = move();

		if (bombSimulator.check(movePoint))
		{
			movePoint = Share::My().point;
			cerr << "danger" << endl;
		}

		if (!bombFlag)
			command = CMove + movePoint.toString();
		else
			command = CBomb + movePoint.toString();

		return command;
	}

private:

	/// <summary>目的地</summary>
	Point destination;

	bool subDestinationFlag = false;
	Point subDestination;

	Search search;
	BombSimulator bombSimulator;

	Grid<int> rangeTable;

	void updateRangeTable() {

		rangeTable.resize(Share::Width(), Share::Height(), INT32_MAX);

		if (!destination) return;

		rangeTable[destination] = 0;

		queue<Point> que;
		que.push(destination);

		while (!que.empty())
		{
			const auto point = que.front();
			que.pop();
			const int r = rangeTable[point];

			for (const auto dire : Direction)
			{
				const auto p = point + dire;
				if (inside(p) && rangeTable[p] == INT32_MAX)
				{
					rangeTable[p] = r + 1;
					que.push(p);
				}
			}

		}
	}

	const Point move() const {

		const Point myPoint = Share::My().point;

		for (const auto& dire : Direction)
		{
			const Point point = myPoint + dire;
			if (inside(point))
				if (rangeTable[point] < rangeTable[myPoint])
					return point;
		}
		return myPoint;
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
