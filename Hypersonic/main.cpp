#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include <queue>
#include <map>
#include <chrono>

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

class Stopwatch {
public:

	void start() {
		s = chrono::system_clock::now();
		e = s;
	}
	void stop() {
		e = chrono::system_clock::now();
	}

	const long long second() const { return chrono::duration_cast<chrono::seconds>(e - s).count(); }
	const long long millisecond() const { return chrono::duration_cast<chrono::milliseconds>(e - s).count(); }
	const long long microseconds() const { return chrono::duration_cast<chrono::microseconds>(e - s).count(); }

private:

	chrono::time_point<chrono::system_clock> s;
	chrono::time_point<chrono::system_clock> e;

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

	inline static const Grid<int>& ItemStage() { return itemStage; }
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
	static Grid<int> itemStage;
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
		itemStage.resize(width, height, 0);
	}

};

int Share::width;
int Share::height;
int Share::myId;

Grid<Table> Share::stage;
vector<vector<bool>> Share::blockStage;
Grid<int> Share::itemStage;
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
				Share::itemStage[Point(x, y)] = param1;
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

	void simulate(Grid<pair<int, int>> bombStage, Grid<int> itemStage, Grid<Table> stage) {

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

	bool next(const Point& myPoint, const Point& nextPoint, Grid<pair<int, int>>& bomb, Grid<int>& item, Grid<Table>& stage) {

		set<Point> danger;
		myBombCount = 0;
		bombList.clear();

		for (const auto& b : Share::MyB()) bombList[b.point] = Share::MyId();

		for (int y = 0; y < Share::Height(); y++)
		{
			for (int x = 0; x < Share::Width(); x++)
			{
				if (bomb[y][x].first == 1)
				{
					auto d = destroy(Point(x, y), bomb, item, stage);
					for (const auto b : d) danger.insert(b);
				}
				bomb[y][x].first--;
			}
		}

		for (const auto& p : danger)
		{
			if (p == myPoint || p == nextPoint) return true;

			if (item[p] > 0)
				item[p] = 0;

			if (stage[p] == Table::ExtraBox)
			{
				stage[p] = Table::Cell;
				item[p] = ItemExtra;
			}
			else if (stage[p] == Table::RangeBox)
			{
				stage[p] = Table::Cell;
				item[p] = ItemRange;
			}
			else if (stage[p] == Table::EmptyBox)
			{
				stage[p] = Table::Cell;
			}
		}

		return false;
	}

	const int getBomb() const { return myBombCount; }

	const int destroyBox(const Point& point, const int blast, Grid<pair<int, int>>& bomb, Grid<int> item, Grid<Table>& stage) const {
		int box = 0;

		const auto insert = [&](const Point& p) {
			if (inside(p))
			{
				if (item[p] != 0 || stage[p] != Table::Cell || bomb[p].first > 0)
				{
					if (isBox(stage[p]))
						box++;

					return true;
				}
			}
			return false;
		};

		for (int dy = 1; dy < blast; dy++)
		{
			const Point nextPoint = point + Point(0, dy);
			if (insert(nextPoint)) break;
		}
		for (int dy = 1; dy < blast; dy++)
		{
			const Point nextPoint = point + Point(0, -dy);
			if (insert(nextPoint)) break;
		}

		for (int dx = 1; dx < blast; dx++)
		{
			const Point nextPoint = point + Point(dx, 0);
			if (insert(nextPoint)) break;
		}
		for (int dx = 1; dx < blast; dx++)
		{
			const Point nextPoint = point + Point(-dx, 0);
			if (insert(nextPoint)) break;
		}


		return box;
	}

private:

	vector<set<Point>> dangers;
	int myBombCount;

	map<Point, int> bombList;

	const set<Point> destroy(const Point& point, Grid<pair<int, int>>& bomb, Grid<int>& item, Grid<Table>& stage) {

		const int r = bomb[point].second;
		set<Point> danger;
		danger.insert(point);
		bomb[point] = { 0,0 };
		if (bombList.find(point) != bombList.end())
			myBombCount++;

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

	const Point boxSearch(const int r = 8) const {

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

int maxBoxRange;
int minBoxRange;

class AI {
public:

	//ビームサーチを実装する
	const string think() {

		const Point Move[] = { Point(0,-1),Point(-1,0),Point(1,0),Point(0,1),Point(0,0) };

		//保存する情報　自機　ステージ(ボム・アイテム・ボックス)
		Data now;
		now.my = Share::My();
		now.stage = Share::Stage();
		now.command.clear();
		now.score = 0;
		now.box = 0;

		now.bomb.resize(Share::Width(), Share::Height(), { 0,0 });
		now.item.resize(Share::Width(), Share::Height(), 0);

		for (const auto& b : Share::MyB()) now.bomb[b.point] = { b.val1,b.val2 };
		for (const auto& b : Share::EnB()) now.bomb[b.point] = { b.val1,b.val2 };

		for (const auto& i : Share::Item()) now.item[i.point] = i.val1;

		priority_queue<Data> que;
		que.push(now);

		const double Decay = 0.9;
		const int Turn = 10;
		for (int turn = 0; turn < Turn; turn++)
		{
			priority_queue<Data> next;

			int width = 0;
			while (!que.empty() && width < 40)
			{
				for (const auto& dire : Move)
				{
					Data d;
					const Point p = que.top().my.point + dire;

					if (inside(p) && que.top().stage[p] == Table::Cell && que.top().bomb[p].first <= 0)
					{
						const auto func = [&]() {
							const Point priP = d.my.point;
							d.my.point = p;
							if (d.item[p] == ItemExtra) d.my.val1++;
							if (d.item[p] == ItemRange) d.my.val2++;
							d.item[p] = 0;
							d.my.val1 += bombSimulator.getBomb();

							d.score += (int)(eval(d, priP) * pow(Decay, turn));

							next.push(d);
						};

						d = que.top();
						if (turn <= Turn - 8 && d.my.val1 > 0)
						{
							d.command.push_back(CBomb + p.toString());
							d.bomb[d.my.point] = { 8,d.my.val2 };
							d.my.val1--;
							d.box += bombSimulator.destroyBox(d.my.point, d.my.val2, d.bomb, d.item, d.stage);
							if (!bombSimulator.next(d.my.point, p, d.bomb, d.item, d.stage))
							{
								func();
							}
						}

						d = que.top();
						d.command.push_back(CMove + p.toString());
						if (!bombSimulator.next(d.my.point, p, d.bomb, d.item, d.stage))
						{
							func();
						}

					}

				}

				que.pop();
				width++;
			}

			que.swap(next);
		}

		if (!que.empty())
		{
			cerr << "Score:" << que.top().score << endl;
			//for (const auto& c : que.top().command) cerr << c << endl;
			return que.top().command[0];
		}

		string command = CMove + Point(Share::Width() / 2, Share::Height() / 2).toString();

		return command;
	}



private:

	Search search;
	BombSimulator bombSimulator;

	struct Data {
		int score;
		int box;
		Entitie my;
		Grid<pair<int, int>> bomb;
		Grid<int> item;
		Grid<Table> stage;
		vector<string> command;

		const bool operator<(const Data& d) const { return score < d.score; }

	};

	const int eval(const Data& data, const Point& p) {
		int s = 0;

		s += data.box * 15;
		s -= data.my.val1 * 5;

		//s += range(Share::My().point, data.my.point);

		s += min(data.my.val1, 7) * 10;
		s += min(data.my.val2, 8) * 6;

		int boxRangeScore = 0;
		for (int y = 0; y < Share::Height(); y++)
		{
			for (int x = 0; x < Share::Width(); x++)
			{
				if (isBox(data.stage[Point(x, y)]))
				{
					const int r = range(data.my.point, Point(x, y));
					boxRangeScore += 600 - (r*r);
				}
			}
		}

		maxBoxRange = max(maxBoxRange, boxRangeScore);
		minBoxRange = min(minBoxRange, boxRangeScore);
		if (boxRangeScore > 0)
		{
			boxRangeScore = max(boxRangeScore, 2500);
			boxRangeScore = min(boxRangeScore, 20000);
		}
		s += boxRangeScore / 100;

		auto d = data;

		for (const auto& enemy : Share::En())
		{
			d.bomb[enemy.point] = { 8,enemy.val2 };
		}
		if (bombSimulator.next(d.my.point, p, d.bomb, d.item, d.stage))
			s = 0;

		return s;
	}

};

int main()
{
	Input::first();

	AI ai;
	Stopwatch sw;

	while (true)
	{
		Input::loop();

		maxBoxRange = 0;
		minBoxRange = INT32_MAX;

		sw.start();
		const string command = ai.think();
		sw.stop();

		cerr << "max:" << maxBoxRange << endl;
		cerr << "min:" << minBoxRange << endl;

		cout << command << " " << sw.millisecond() << "ms" << endl;
	}
}
