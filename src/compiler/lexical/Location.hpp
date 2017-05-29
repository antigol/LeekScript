#ifndef LOCATION_HPP_
#define LOCATION_HPP_

namespace ls {

struct Position {
	size_t line;
	size_t column;
	size_t raw;
	Position(size_t line, size_t column, size_t raw) :
		line(line), column(column), raw(raw) {}
};

struct Location {
	Position start;
	Position end;
	Location(Position start, Position end) : start(start), end(end) {}
};

}

#endif
