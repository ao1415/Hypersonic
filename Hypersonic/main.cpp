#include <iostream>

#include <string>

#include <vector>
#include <array>
#include <set>
#include <map>
#include <queue>
#include <stack>

#include <chrono>
#include <algorithm>
#include <memory>

using namespace std;

#define forange(counter, end) forstep(counter, 0, end)
#define forstep(counter, begin, end) forstep_type(size_t, counter, begin, end)
#define forange_type(type, counter, end) forstep_type(type, counter, 0, end)
#define forstep_type(type, counter, begin, end) for (type counter = begin, forstep_type_end_##counter = end; counter < forstep_type_end_##counter; counter++)

#pragma region �萔�錾

namespace Object {

	const char Empty = '.';
	const char EmptyBox = '0';
	const char RangeBox = '1';
	const char BombBox = '2';
	const char Wall = 'X';

	const int Player = 0;
	const int Bomb = 1;
	const int Item = 2;

	const int ItemRange = 1;
	const int ItemBomb = 2;

	const int Width = 13;
	const int Height = 11;

	const int MaxRounds = 200;

	const int MaxSearchTurn = 12;
	const int MaxBombTurn = 8;

	enum class Cell {
		/// <summary>��</summary>
		Empty,
		/// <summary>��</summary>
		BoxEmpty,
		/// <summary>�Η͔�</summary>
		BoxRange,
		/// <summary>�{����</summary>
		BoxBomb,
		/// <summary>��</summary>
		Wall,
		/// <summary>�ἨA�C�e��</summary>
		ItemRange,
		/// <summary>�{���A�C�e��</summary>
		ItemBomb,
		Size
	};
}

#pragma endregion

#pragma region �f�[�^�\��
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

	const bool operator<(const Point& p) const { return ((y << 8) + x < (p.y << 8) + p.x); }

};

template<typename Type, size_t Width = Object::Width, size_t Height = Object::Height>
class FixedGrid {
private:

	Type m_data[Width * Height];

public:

	FixedGrid() = default;
	FixedGrid(const Type& v) { fill(v); }
	FixedGrid(const FixedGrid&) = default;
	FixedGrid(FixedGrid&&) = default;

	FixedGrid& operator=(const FixedGrid& other) = default;
	FixedGrid& operator=(FixedGrid&& other) = default;

	const Type* operator[](size_t y) const { return &m_data[y * Width]; }
	Type* operator[](size_t y) { return &m_data[y * Width]; }

	const Type& operator[](const Point& p) const { return m_data[p.y * Width + p.x]; }
	Type& operator[](const Point& p) { return m_data[p.y * Width + p.x]; }

	const Type& at(size_t x, size_t y) const {
		if (outside(x, y))
			throw std::out_of_range("FixedGrid::at");
		return m_data[y * Width + x];
	}
	Type& at(size_t x, size_t y) {
		if (outside(x, y))
			throw std::out_of_range("FixedGrid::at");
		return m_data[y * Width + x];
	}

	constexpr size_t width() const { return Width; }
	constexpr size_t height() const { return Height; }

	bool inside(size_t x, size_t y) const { return (0 <= x && x < Width && 0 <= y && y < Height); }
	bool outside(size_t x, size_t y) const { return (0 > x || x >= Width || 0 > y || y >= Height); }

	void fill(const Type& v) noexcept { std::fill_n(m_data, Width * Height, v); }

};

struct Entitie {
	Entitie() {}
	Entitie(const Point& p, const int v1, const int v2) {
		pos = p; val1 = v1; val2 = v2;
	}

	int id = -1;
	Point pos;
	int val1 = 0;
	int val2 = 0;
};
using Entities = vector<Entitie>;

template<typename Type = int>
class Sequencer {
private:

	inline static Type counter = 0;

public:

	Type Get() {
		const auto n = counter;
		counter++;
		return n;
	}

};
#pragma endregion

#pragma region ���C�u����

/// <summary>
	/// ���Ԍv�����s���N���X
	/// </summary>
class Stopwatch {
public:

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	Stopwatch() = default;

	/// <summary>
	/// �v�����J�n����
	/// </summary>
	inline void start() noexcept {
		s = std::chrono::high_resolution_clock::now();
		e = s;
	}
	/// <summary>
	/// �v�����~����
	/// </summary>
	inline void stop() noexcept {
		e = std::chrono::high_resolution_clock::now();
	}

	/// <summary>
	/// �v�����Ԃ��擾����(�i�m�b)
	/// </summary>
	/// <returns>�v������(�i�m�b)</returns>
	inline const long long nanoseconds() const noexcept { return std::chrono::duration_cast<std::chrono::nanoseconds>(e - s).count(); }
	/// <summary>
	/// �v�����Ԃ��擾����(�}�C�N���b)
	/// </summary>
	/// <returns>�v������(�}�C�N���b)</returns>
	inline const long long microseconds() const noexcept { return std::chrono::duration_cast<std::chrono::microseconds>(e - s).count(); }
	/// <summary>
	/// �v�����Ԃ��擾����(�~���b)
	/// </summary>
	/// <returns>�v������(�~���b)</returns>
	inline const long long millisecond() const noexcept { return std::chrono::duration_cast<std::chrono::milliseconds>(e - s).count(); }

	/// <summary>
	/// �P�ʕt���̌v�����Ԃ̕�����𓾂�(�i�m�b)
	/// </summary>
	/// <returns>�v�����Ԃ̕�����(�i�m�b)</returns>
	inline const std::string toString_ns() const { return std::to_string(nanoseconds()) + "ns"; }
	/// <summary>
	/// �P�ʕt���̌v�����Ԃ̕�����𓾂�(�}�C�N���b)
	/// </summary>
	/// <returns>�v�����Ԃ̕�����(�}�C�N���b)</returns>
	inline const std::string toString_us() const { return std::to_string(microseconds()) + "us"; }
	/// <summary>
	/// �P�ʕt���̌v�����Ԃ̕�����𓾂�(�~���b)
	/// </summary>
	/// <returns>�v�����Ԃ̕�����(�~���b)</returns>
	inline const std::string toString_ms() const { return std::to_string(millisecond()) + "ms"; }

private:

	std::chrono::time_point<std::chrono::high_resolution_clock> s;
	std::chrono::time_point<std::chrono::high_resolution_clock> e;

};

/// <summary>
/// ��莞�Ԃ̌o�߂��m�F����N���X
/// </summary>
class MilliSecTimer {
private:

	std::chrono::time_point<std::chrono::high_resolution_clock> s;
	unsigned long long int startCycle = 0;
	long long time = 0;

	const double CyclePerMilliSec = 2794000.0;

#ifndef _MSC_VER
	unsigned long long int getCycle() const {
		unsigned int low, high;
		__asm__ volatile ("rdtsc" : "=a" (low), "=d" (high));
		return ((unsigned long long int)low) | ((unsigned long long int)high << 32);
	}
#endif // _MSC_VER

public:

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	MilliSecTimer() = default;
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="_time">�ݒ莞��(�~���b)</param>
	MilliSecTimer(const std::chrono::milliseconds& _time) noexcept { time = _time.count(); }

	/// <summary>
	/// ���Ԃ�ݒ肷��
	/// </summary>
	/// <param name="_time">�ݒ莞��(�~���b)</param>
	void set(const std::chrono::milliseconds& _time) noexcept { time = _time.count(); }

	/// <summary>
	/// �^�C�}�[���J�n������
	/// </summary>
	void start() noexcept {
#ifdef _MSC_VER
		s = std::chrono::high_resolution_clock::now();
#else
		startCycle = getCycle();
#endif // _MSC_VER
	}

	/// <summary>
	/// �ݒ莞�Ԍo�߂������𓾂�
	/// </summary>
	/// <returns>�o�߂��Ă���� true, ����ȊO�� false</returns>
	inline const bool check() const noexcept {
#ifdef _MSC_VER
		const auto e = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<std::chrono::milliseconds>(e - s).count() >= time;
#else
		return (getCycle() - startCycle) / CyclePerMilliSec >= time;
#endif // _MSC_VER
	}

	/// <summary>
	/// �ݒ莞�Ԍo�߂������𓾂�
	/// </summary>
	/// <returns>�o�߂��Ă���� true, ����ȊO�� false</returns>
	operator bool() const noexcept { return check(); }

	/// <summary>
	/// �o�ߎ��Ԃ��擾����(�~���b)
	/// </summary>
	/// <returns>�v������(�~���b)</returns>
	inline const long long interval() const noexcept {
#ifdef _MSC_VER
		const auto e = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<std::chrono::milliseconds>(e - s).count();
#else
		return static_cast<long long int>((getCycle() - startCycle) / CyclePerMilliSec);
#endif // _MSC_VER
	}

};

#pragma endregion

#pragma region ���L�f�[�^

class Input;

class Share {
public:

	friend Input;

	static void Create() {
		instance.reset(new Share());
	}

	static Share& Get() {
		return *instance;
	}

	//�ȉ��ɕK�v��getter���L��

	const auto getMyid() const { return myid; }
	const auto& getStage() const { return stage; }
	const auto& getBoxLimit() const { return boxLimit; }
	const auto& getBombLimit() const { return bombLimit; }
	const auto& getItemLimit() const { return itemLimit; }

	const auto& getBombTable() const { return bombTable; }
	const auto& getItemTable() const { return itemTable; }

	const auto& getMy() const { return my; }
	const auto& getEnemy() const { return enemy; }

	const auto& getBomb() const { return bomb; }
	const auto& getItem() const { return item; }

private:

	Share() {}

	inline static shared_ptr<Share> instance;

	//�ȉ��ɕK�v�ȃf�[�^���L��

	int myid = -1;
	FixedGrid<Object::Cell> stage;

	FixedGrid<int> boxLimit;
	FixedGrid<int> bombLimit;
	FixedGrid<int> itemLimit;

	FixedGrid<int> bombTable;
	FixedGrid<int> itemTable;

	Entitie my;
	Entities enemy;

	Entities bomb;
	Entities item;
};

#pragma endregion

#pragma region �f�[�^����

class Input {
private:

public:

	Input() {}

	void first() {

		auto& share = Share::Get();

		int w, h;
		cin >> w >> h >> share.myid;
		cin.ignore();

	}

	void loop() {

		auto& share = Share::Get();
		auto& stage = share.stage;

		share.boxLimit.fill(0);

		forange(y, stage.height())
		{
			forange(x, stage.width())
			{
				using namespace Object;
				char c;
				if (!(cin >> c)) exit(0);
				switch (c)
				{
				case Empty: stage[y][x] = Cell::Empty; break;
				case EmptyBox: stage[y][x] = Cell::BoxEmpty; share.boxLimit[y][x] = MaxRounds; break;
				case RangeBox: stage[y][x] = Cell::BoxRange; share.boxLimit[y][x] = MaxRounds; break;
				case BombBox: stage[y][x] = Cell::BoxBomb; share.boxLimit[y][x] = MaxRounds; break;
				case Wall: stage[y][x] = Cell::Wall; break;
				default: stage[y][x] = Cell::Empty; break;
				}
			}
			cin.ignore();
		}

		int entitieSize;
		cin >> entitieSize;
		cin.ignore();

		share.enemy.clear();
		share.bomb.clear();
		share.item.clear();

		forange(i, entitieSize)
		{
			int type;
			int owner;
			Entitie entitie;

			cin >> type >> owner >> entitie.pos.x >> entitie.pos.y >> entitie.val1 >> entitie.val2;
			cin.ignore();

			using namespace Object;

			switch (type)
			{
			case Player:
				if (owner == share.myid) share.my = entitie;
				else share.enemy.push_back(entitie);
				break;
			case Bomb:
				share.bomb.push_back(entitie);
				break;
			case Item:
				share.item.push_back(entitie);
				break;
			default:
				break;
			}

		}

		//�e�[�u��������

		share.bombLimit.fill(0);
		share.bombTable.fill(0);
		share.itemLimit.fill(0);
		share.itemTable.fill(0);

		for (const auto& bomb : share.bomb)
		{
			share.bombLimit[bomb.pos] = bomb.val1; //�J�E���g
			share.bombTable[bomb.pos] = bomb.val2; //�����W
		}
		for (const auto& item : share.item)
		{
			share.itemLimit[item.pos] = Object::MaxRounds;
			share.itemTable[item.pos] = item.val1;
		}

	}

};

#pragma endregion

#pragma region ���[�e�B���e�B

const string format() {
	return "";
}

ostream& operator<<(ostream& os, const Point& p) {
	os << "(" << p.x << ", " << p.y << ")";
	return os;
}

namespace Command {

	const string MoveCommandStr = "MOVE ";
	const string BombCommandStr = "BOMB ";

	const string move(const int x, const int y) { return MoveCommandStr + to_string(x) + " " + to_string(y); }
	const string move(const Point& pos) { return MoveCommandStr + to_string(pos.x) + " " + to_string(pos.y); }

	const string bomb(const int x, const int y) { return BombCommandStr + to_string(x) + " " + to_string(y); }
	const string bomb(const Point& pos) { return BombCommandStr + to_string(pos.x) + " " + to_string(pos.y); }
}

bool inside(const int x, const int y) { return (0 <= x && x < Object::Width && 0 <= y && y < Object::Height); }
bool inside(const Point& p) { return (0 <= p.x && p.x < Object::Width && 0 <= p.y && p.y < Object::Height); }

template<class T>
void debug(const T& mes) {
	cerr << mes << endl;
}

#pragma endregion

#pragma region AI

struct Data {

};

class Engine {
private:

	inline static FixedGrid<Object::Cell> stage;

	FixedGrid<bool> box;
	FixedGrid<bool> item;
	FixedGrid<std::uint8_t> bomb;//0xf0:�J�E���g�@0x0f:�͈�
	FixedGrid<bool> blast;

public:

	static Engine Create() {

		const auto& share = Share::Get();
		stage = share.getStage();
		const auto& boxLimit = share.getBoxLimit();
		const auto& itemLimit = share.getItemLimit();

		const auto& bombLimit = share.getBombLimit();
		const auto& bombTable = share.getBombTable();

		Engine engine;

		forange_type(int, y, Object::Height)
		{
			forange_type(int, x, Object::Width)
			{
				const auto& cell = stage[y][x];
				engine.box[y][x] = (boxLimit[y][x] != 0);
				engine.item[y][x] = (itemLimit[y][x] != 0);
				engine.bomb[y][x] = (static_cast<std::uint8_t>(bombLimit[y][x] << 4) | (bombTable[y][x] & 0x0F));
			}
		}

		return engine;
	}

	Engine() {
		box.fill(false);
		item.fill(false);
		bomb.fill(0);
		blast.fill(false);
	}

	Engine(const FixedGrid<bool>& box, const FixedGrid<bool>& item, const FixedGrid<std::uint8_t>& bomb) {
		this->box = box;
		this->item = item;
		this->bomb = bomb;
		blast.fill(false);
	}

	Engine nextEngine() const {

		Engine next(box, item, bomb);

		queue<Point> que;

		//�X�V����
		const auto tableUpdate = [&](const Point& pos) {

			//�{�b�N�X�X�V����
			if (box[pos])
			{
				//�A�C�e���{�b�N�X����A�C�e���𐶐�
				if (stage[pos] != Object::Cell::BoxEmpty)
				{
					next.item[pos] = true;
				}

				//���^�[������{�b�N�X�폜
				next.box[pos] = false;
				return true;
			}

			//�{���U������
			if ((next.bomb[pos] & 0xF0) > 0)
			{
				//�{���̎������X�V
				next.bomb[pos] &= 0x0F;
				que.push(pos);
				return true;
			}

			//�A�C�e���X�V����
			if (next.item[pos])
			{
				//���^�[������A�C�e���폜
				next.item[pos] = false;
				return true;
			}
			return false;
		};

		forange_type(int, y, Object::Height)
		{
			forange_type(int, x, Object::Width)
			{
				if ((next.bomb[y][x] & 0xF0) == 0x10)
				{
					next.bomb[y][x] &= 0x0F;
					que.push(Point(x, y));
				}
			}
		}

		while (!que.empty())
		{
			const Point pos = que.front();
			que.pop();

			next.blast[pos] = true;
			const auto range = next.bomb[pos];

			//��
			for (int dy = pos.y - 1, end = max(pos.y - range, 0); dy >= end; dy--)
			{
				const Point updatePos(pos.x, dy);
				next.blast[updatePos] = true;
				if (tableUpdate(updatePos)) break;
			}
			//��
			for (int dy = pos.y + 1, end = min(pos.y + range, Object::Height - 1); dy <= end; dy++)
			{
				const Point updatePos(pos.x, dy);
				next.blast[updatePos] = true;
				if (tableUpdate(updatePos)) break;
			}

			//��
			for (int dx = pos.x - 1, end = max(pos.x - range, 0); dx >= end; dx--)
			{
				const Point updatePos(dx, pos.y);
				next.blast[updatePos] = true;
				if (tableUpdate(updatePos)) break;
			}
			//�E
			for (int dx = pos.x + 1, end = min(pos.x + range, Object::Width - 1); dx <= end; dx++)
			{
				const Point updatePos(dx, pos.y);
				next.blast[updatePos] = true;
				if (tableUpdate(updatePos)) break;
			}
		}

		//�J�E���g�_�E��
		forange_type(int, y, Object::Height)
		{
			forange_type(int, x, Object::Width)
			{
				if ((next.bomb[y][x] & 0xF0) > 0)
				{
					next.bomb[y][x] -= 0x10;
				}
			}
		}

		return next;
	}

	bool isBlast(const Point& pos) const {
		return blast[pos];
	}

};

class AI {
private:

	inline static const Point dist[5] = { Point(1,0),Point(0,1),Point(-1,0),Point(0,-1),Point(0,0) };

public:

	string think() {

		const auto& share = Share::Get();
		const Entitie my = share.getMy();

		const Engine engine = Engine::Create();

		const auto& next = engine.nextEngine();

		for (const auto& d : dist)
		{
			const Point pos = my.pos + d;

			if (inside(pos) && !next.isBlast(pos))
			{
				return Command::move(pos);
			}
		}

		return "WAIT �l�݂ł�(�L�E�ցE`)";
	}

};

#pragma endregion

#pragma region �f�[�^�o��

int main() {

	Share::Create();

	Input input;
	input.first();

	Stopwatch sw;

	AI ai;

	while (true)
	{
		input.loop();

		sw.start();
		const auto& coms = ai.think();
		//const auto& coms = "WAIT";
		sw.stop();

		cout << coms << endl;

		cerr << sw.toString_ms() << endl;
	}

	return 0;
}

#pragma endregion
