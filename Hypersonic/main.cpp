#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include <queue>
#include <map>
#include <chrono>
#include <array>

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

const int Width = 13;
const int Height = 11;

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
	const std::array<Type, Width>& operator[](int y) const { return grid[y]; }
	std::array<Type, Width>& operator[](int y) { return grid[y]; }

	const Size& size() const { return s; }
	void resize(int w, int h) { resize(Size(w, h)); }
	void resize(const Size& s) {
		//grid.resize(s.h);
		//for (auto& g : grid) g.resize(s.w);
		this->s = s;
	}
	void resize(int w, int h, const Type v) { resize(Size(w, h), v); }
	void resize(const Size& s, const Type v) {
		//grid.resize(s.h);
		//for (auto& g : grid) g.resize(s.w, v);
		for (auto& g : grid) g.fill(v);
		this->s = s;
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

	void fill(const Type& v) {
		for (auto& g : grid) g.fill(v);
	}

private:

	std::array<std::array<Type, Width>, Height> grid;
	Size s;

};

class Stopwatch {
public:

	void start() {
		s = chrono::high_resolution_clock::now();
		e = s;
	}
	void stop() {
		e = chrono::high_resolution_clock::now();
	}

	const long long second() const { return chrono::duration_cast<chrono::seconds>(e - s).count(); }
	const long long millisecond() const { return chrono::duration_cast<chrono::milliseconds>(e - s).count(); }
	const long long microseconds() const { return chrono::duration_cast<chrono::microseconds>(e - s).count(); }

private:

	chrono::time_point<chrono::high_resolution_clock> s;
	chrono::time_point<chrono::high_resolution_clock> e;

};

class Timer {
public:

	static const int Second = 1;
	static const int MilliSecond = 2;
	static const int MicroSecond = 3;

	Timer() = default;
	Timer(const long long _time, const int _type) {
		setTimer(_time, _type);
	}

	void setTimer(const long long _time, const int _type) {
		time = _time;
		type = _type;
	}

	void start() { s = chrono::high_resolution_clock::now(); }

	inline const bool check() const {
		const auto e = chrono::high_resolution_clock::now();
		long long t = 0;
		switch (type)
		{
		case Second:
			t = chrono::duration_cast<chrono::seconds>(e - s).count();
			break;
		case MilliSecond:
			t = chrono::duration_cast<chrono::milliseconds>(e - s).count();
			break;
		case MicroSecond:
			t = chrono::duration_cast<chrono::microseconds>(e - s).count();
			break;
		}
		return t >= time;
	}

	operator bool() const { return check(); }

private:

	chrono::time_point<chrono::high_resolution_clock> s;
	long long time = 0;
	int type;

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
		//cerr << Share::width << " " << Share::height << " " << Share::myId << endl;
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
				if (!(cin >> c)) exit(0);
				//cin >> c;
				//cerr << c;
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
			//cerr << endl;
		}
		int entities;
		cin >> entities; cin.ignore();
		//cerr << entities << endl;

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
			//cerr << entityType << " " << owner << " " << x << " " << y << " " << param1 << " " << param2 << endl;

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
			//if (p == myPoint || p == nextPoint) return true;

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

		if (danger.find(myPoint) != danger.end() || danger.find(nextPoint) != danger.end())
			return true;

		return false;
	}

	const vector<Grid<int>> getBlastGrid(Grid<pair<int, int>> bomb, Grid<int> item, Grid<Table> stage, const int turn) {

		vector<Grid<int>> blastGrid(turn);
		for (auto& grid : blastGrid) grid.fill(0);

		for (int i = 0; i < turn; i++)
		{
			set<Point> danger;

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
				blastGrid[i][p] = 1;
			}

		}

		return blastGrid;
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

	const int destroyBoxCount(Grid<pair<int, int>> bomb, Grid<int> item, Grid<Table> stage) const {

		for (int turn = 0; turn < 8; turn++)
		{
			for (int y = 0; y < Share::Width(); y++)
			{
				for (int x = 0; x < Share::Height(); x++)
				{

				}
			}
		}

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

int maxBoxRange;
int minBoxRange;

long long maxSimulatorTime;
long long minSimulatorTime;

class AI {
public:

	//chokudaiサーチ
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

		/*
		for (const auto& enemy : Share::En())
		{
		if (enemy.val1 > 0)
		now.bomb[enemy.point] = { 8,enemy.val2 };
		}
		*/

		const auto check = [](const Point& point, const vector<Grid<int>>& blast, const Grid<pair<int, int>>& bomb, const Grid<Table>& stage, const int turn) {

			queue<Point> que;

			que.push(point);

			set<Point> points;
			points.insert(point);

			for (size_t i = turn; i < blast.size() - 1; i++)
			{
				queue<Point> nque;
				set<Point> nextPoints;

				for (const auto& p : points)
				{
					for (const auto& dire : Direction)
					{
						const Point nextP = p + dire;
						if (inside(nextP) && bomb[nextP].first == 0 && stage[nextP] == Table::Cell)
						{
							if (blast[i][nextP] == 0 && blast[i + 1][nextP] == 0)
								nextPoints.insert(nextP);
						}
					}
					if (blast[i + 1][p] == 0)
						nextPoints.insert(p);
				}
				points.swap(nextPoints);

			}

			return !que.empty();
		};

		const double Decay = 1.0;

		const int Turn = 10;
		const int ChokudaiWidth = 5;

		const auto blastGrid = bombSimulator.getBlastGrid(now.bomb, now.item, now.stage, Turn);

		array<priority_queue<Data>, Turn> qData;
		qData[0].push(now);

		Timer timer(90, Timer::MilliSecond);
		timer.start();
		while (!timer)
		{
			for (int turn = 0; turn < Turn - 1; turn++)
			{
				for (int i = 0; i < ChokudaiWidth; i++)
				{
					if (qData[turn].empty())
						break;

					for (const auto& dire : Move)
					{
						Data d;
						const Point p = qData[turn].top().my.point + dire;

						if (inside(p) && qData[turn].top().stage[p] == Table::Cell && qData[turn].top().bomb[p].first <= 0)
						{
							const auto func = [&]() {
								const Point priP = d.my.point;
								d.my.point = p;
								if (d.item[p] == ItemExtra) d.my.val1++;
								if (d.item[p] == ItemRange) d.my.val2++;
								d.item[p] = 0;
								d.my.val1 += bombSimulator.getBomb();

								d.score += (int)(eval(d, priP) * pow(Decay, turn));

								qData[turn + 1].emplace(d);
							};

							d = qData[turn].top();
							if (turn <= Turn - 8 && d.my.val1 > 0)
							{
								d.command.push_back(CBomb + p.toString());
								d.bomb[d.my.point] = { 8,d.my.val2 };
								d.my.val1--;
								d.box += bombSimulator.destroyBox(d.my.point, d.my.val2, d.bomb, d.item, d.stage);
								if (!bombSimulator.next(d.my.point, p, d.bomb, d.item, d.stage))
								{
									if (check(p, blastGrid, d.bomb, d.stage, turn))
									{
										func();
									}
									else
									{
										d.score *= 0.8;
										func();
										cerr << "詰み防止のはず" << endl;
									}
								}
							}

							d = qData[turn].top();
							d.command.push_back(CMove + p.toString());
							if (!bombSimulator.next(d.my.point, p, d.bomb, d.item, d.stage))
							{
								if (check(p, blastGrid, d.bomb, d.stage, turn))
								{
									func();
								}
								else
								{
									d.score *= 0.8;
									func();
									cerr << "詰み防止のはず" << endl;
								}
							}

						}

					}

					qData[turn].pop();
				}
			}
		}

		if (!qData[Turn - 1].empty())
		{
			cerr << "Score:" << qData[Turn - 1].top().score << endl;
			//for (const auto& c : qData[Turn - 1].top().command) cerr << c << endl;
			return qData[Turn - 1].top().command[0];
		}

		string command = CMove + Point(Share::Width() / 2, Share::Height() / 2).toString();
		cerr << "詰みです" << endl;
		return command;
	}

private:

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

		s += data.box * 15 * 100;
		s -= data.my.val1 * 5 * 100;

		s += min(data.my.val1, 7) * 10 * 100;
		s += min(data.my.val2, 8) * 6 * 100;

		int playerRange = INT32_MAX;
		for (const auto& player : Share::En())
			playerRange = min(playerRange, range(player.point, data.my.point));

		s += playerRange * 2 * 50;
		s += range(Share::My().point, data.my.point) * 2 * 50;

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
			boxRangeScore = max(boxRangeScore, 0);
			boxRangeScore = min(boxRangeScore, 20000);
		}
		s += boxRangeScore / 100 * 100;

		auto d = data;

		for (const auto& enemy : Share::En())
		{
			d.bomb[enemy.point] = { 8,enemy.val2 };
		}

		Stopwatch sw;
		sw.start();
		if (bombSimulator.next(d.my.point, p, d.bomb, d.item, d.stage))
			s = 0;
		sw.stop();

		maxSimulatorTime = max(maxSimulatorTime, sw.microseconds());
		minSimulatorTime = min(minSimulatorTime, sw.microseconds());

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
		maxSimulatorTime = 0;
		minSimulatorTime = INT32_MAX;

		sw.start();
		const string command = ai.think();
		sw.stop();

		cerr << "max:" << maxSimulatorTime << endl;
		cerr << "min:" << minSimulatorTime << endl;

		cout << command << " " << sw.millisecond() << "ms" << endl;
	}
}
