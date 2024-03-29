#pragma once
#include "../unit_test.hpp"

import p3.grid;

#pragma region helper functions

namespace grid_test
{
	template <typename data_type, size_t size>
	void assert_equal_grids(const p3::grid<data_type, size> &expected, const p3::grid<data_type, size> &actual, const std::string &name)
	{
		// todo: create a universal equality_asserter<type> in unit test and convert this into a specialization of it.

		const auto append_optional = [](const std::string &optional, const std::string &append)
		{
			if (optional.empty())
			{
				return append;
			}
			return optional + ": " + append;
		};

		if (actual.dim() != expected.dim())
		{
			unit_test::assert_equals(0, 1, append_optional(name, "dimensions did not match"));
		}

		unit_test::assert_equals(actual.size(), expected.size(), append_optional(name, "serious error: size did not match, despite dimensions matching"));

		auto actual_iter = actual.begin();
		auto expected_iter = expected.begin();

		size_t index = 0;
		for (; actual_iter != actual.end() && expected_iter != expected.end(); ++actual_iter, ++expected_iter)
		{
			unit_test::assert_equals<data_type>(*expected_iter, *actual_iter, append_optional(name, std::format("value mismatch at grid[{}]", index)));
			++index;
		}
	}

	template <typename data_type, size_t dimensions>
	void assert_subgrid_axis(const p3::grid<data_type, dimensions + 1> &grid, const std::vector<p3::grid<data_type, dimensions>> &vec, size_t axis)
	{
		for (size_t index = 0; index < vec.size(); ++index)
		{
			// assert_subgrid(vec[index], grid.subgrid(index, axis), index, axis);
			assert_equal_grids(vec[index], grid.subgrid(index, axis), std::format("grid::subgrid({0}, {1})", index, axis));
		}
	}
}

#pragma endregion
#pragma region nested_list

/*P3_UNIT_TEST(grid_nested_lists)
{
	// apparently the IDE is not very happy with the type shenannigans that I did in the module.
	// it's perfectly fine for the compiler, though.

	p3::nested_list_t<int, 0> list_0 = 1;
	p3::nested_list_t<int, 1> list_1 = { 1, 2, 3, 4 };
	p3::nested_list_t<int, 2> list_2 = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 } };
	p3::nested_list_t<int, 3> list_3 = { { { 1, 2 }, { 3, 4 } }, { { 5, 6 }, { 7, 8 } } };
}*/

#pragma endregion
#pragma region grid_size

P3_UNIT_TEST(grid_size_elements)
{
	constexpr p3::grid_size<4> size{ 2, 3, 5, 7 };
	unit_test::assert_equals<size_t>(210U, size.elements(), "grid_size::elements()");
}

P3_UNIT_TEST(grid_size_elements_partial)
{
	constexpr p3::grid_size<5> size{ 11, 7, 5, 3, 2 };
	unit_test::assert_equals<size_t>(   1, size.elements(5), "grid_size::elements(5)");
	unit_test::assert_equals<size_t>(   2, size.elements(4), "grid_size::elements(4)");
	unit_test::assert_equals<size_t>(   6, size.elements(3), "grid_size::elements(3)");
	unit_test::assert_equals<size_t>(  30, size.elements(2), "grid_size::elements(2)");
	unit_test::assert_equals<size_t>( 210, size.elements(1), "grid_size::elements(1)");
	unit_test::assert_equals<size_t>(2310, size.elements(0), "grid_size::elements(0)");
}

P3_UNIT_TEST(grid_size_index)
{
	constexpr p3::grid_size<3> boundary{ 5, 4, 3 };
	static_assert( 0 == boundary.index_of({ 0, 0, 0 }), "grid_size::index_of({ 0, 0, 0 })");
	static_assert( 1 == boundary.index_of({ 0, 0, 1 }), "grid_size::index_of({ 0, 0, 1 })");
	static_assert( 3 == boundary.index_of({ 0, 1, 0 }), "grid_size::index_of({ 0, 1, 0 })");
	static_assert(12 == boundary.index_of({ 1, 0, 0 }), "grid_size::index_of({ 1, 0, 0 })");
	static_assert(16 == boundary.index_of({ 1, 1, 1 }), "grid_size::index_of({ 1, 1, 1 })");
	static_assert(59 == boundary.index_of({ 4, 3, 2 }), "grid_size::index_of({ 4, 3, 2 })");
	static_assert( 7 == boundary.index_of({ 0, 2, 1 }), "grid_size::index_of({ 0, 2, 1 })");

	for (size_t i = 0; i < boundary.elements(); ++i)
	{
		const auto position = p3::grid_size<3>::from_index(i, boundary);
		const auto index = position.index_in(boundary);
		unit_test::assert_equals<size_t>(i, index, "grid_size::index_in()");
	}
}

P3_UNIT_TEST(grid_size_fitting)
{
	constexpr p3::grid_size<2> size{ 1, 5 };
	constexpr auto elements = size.elements();

	static_assert(1 == size.fit_to_data(4)[0], "grid_size::fit_to_data(4) did not reach the expected amount of rows");
	static_assert(1 == size.fit_to_data(5)[0], "grid_size::fit_to_data(5) did not reach the expected amount of rows");
	static_assert(2 == size.fit_to_data(6)[0], "grid_size::fit_to_data(6) did not reach the expected amount of rows");
}

P3_UNIT_TEST(grid_size_remove_axis)
{
	constexpr p3::grid_size<10> size{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	constexpr p3::grid_size<9> rem_0{    1, 2, 3, 4, 5, 6, 7, 8, 9 };
	constexpr p3::grid_size<9> rem_1{ 0,    2, 3, 4, 5, 6, 7, 8, 9 };
	constexpr p3::grid_size<9> rem_2{ 0, 1,    3, 4, 5, 6, 7, 8, 9 };
	constexpr p3::grid_size<9> rem_3{ 0, 1, 2,    4, 5, 6, 7, 8, 9 };
	constexpr p3::grid_size<9> rem_4{ 0, 1, 2, 3,    5, 6, 7, 8, 9 };
	constexpr p3::grid_size<9> rem_5{ 0, 1, 2, 3, 4,    6, 7, 8, 9 };
	constexpr p3::grid_size<9> rem_6{ 0, 1, 2, 3, 4, 5,    7, 8, 9 };
	constexpr p3::grid_size<9> rem_7{ 0, 1, 2, 3, 4, 5, 6,    8, 9 };
	constexpr p3::grid_size<9> rem_8{ 0, 1, 2, 3, 4, 5, 6, 7,    9 };
	constexpr p3::grid_size<9> rem_9{ 0, 1, 2, 3, 4, 5, 6, 7, 8    };


	static_assert(rem_0 == size.remove_axis(0), "grid_size::remove_axis(0) did not produce the desired result");
	static_assert(rem_1 == size.remove_axis(1), "grid_size::remove_axis(1) did not produce the desired result");
	static_assert(rem_2 == size.remove_axis(2), "grid_size::remove_axis(2) did not produce the desired result");
	static_assert(rem_3 == size.remove_axis(3), "grid_size::remove_axis(3) did not produce the desired result");
	static_assert(rem_4 == size.remove_axis(4), "grid_size::remove_axis(4) did not produce the desired result");
	static_assert(rem_5 == size.remove_axis(5), "grid_size::remove_axis(5) did not produce the desired result");
	static_assert(rem_6 == size.remove_axis(6), "grid_size::remove_axis(6) did not produce the desired result");
	static_assert(rem_7 == size.remove_axis(7), "grid_size::remove_axis(7) did not produce the desired result");
	static_assert(rem_8 == size.remove_axis(8), "grid_size::remove_axis(8) did not produce the desired result");
	static_assert(rem_9 == size.remove_axis(9), "grid_size::remove_axis(9) did not produce the desired result");
}

#pragma endregion
#pragma region grid_pos

P3_UNIT_TEST(grid_pos_constructor)
{
	constexpr p3::grid_size<3> empty_size{};
	constexpr p3::grid_size<3> size = { 3, 5, 7 };

	constexpr p3::grid_pos<3> default_pos{};
	constexpr p3::grid_pos<3> size_pos{ size };

	static_assert(default_pos.valid(), "grid_pos::valid()");
	static_assert(default_pos.pos() == empty_size, "grid_pos::pos()");
	static_assert(default_pos.dim() == empty_size, "grid_pos::dim()");

	static_assert(size_pos.valid(), "grid_pos::valid()");
	static_assert(size_pos.pos() == empty_size, "grid_pos::pos()");
	static_assert(size_pos.dim() ==       size, "grid_pos::dim()");
}

P3_UNIT_TEST(grid_pos_comparison)
{
	constexpr p3::grid_size<4> size = { 3, 4, 5, 3 };
	constexpr p3::grid_size<4> pos = { 2, 2, 1, 2 };

	constexpr p3::grid_pos<4> direct{ size, pos };
	static_assert(direct == direct, "grid_pos: object did not equal itself");

	p3::grid_pos<4> jump{ size };
	if (jump != jump)
	{
		unit_test::assert_equals(0, 1, "grid_pos: object did not equal itself");
	}
	if (direct == jump)
	{
		unit_test::assert_equals(0, 1, "grid_pos: two differently valued object achieved equality");
	}

	jump.jump(pos);
	if (direct != jump)
	{
		unit_test::assert_equals(0, 1, "grid_pos: two equaly valued object did not achieve equality");
	}
}

P3_UNIT_TEST(grid_pos_jump)
{
	p3::grid_pos<3> pos{ {2, 4, 8} };

	const auto assert_jump = [&](bool expected, const p3::grid_size<3> &to_pos)
	{
		const auto actual = pos.jump(to_pos);
		unit_test::assert_equals<bool>(expected, actual, std::format("grid_pos::jump({{{0}, {1}, {2}}})", to_pos[0], to_pos[1], to_pos[2]));
	};

	assert_jump(true, { 0, 0, 0 });
	assert_jump(true, { 1, 1, 1 });
	assert_jump(true, { 1, 2, 3 });
	assert_jump(true, { 1, 3, 2 });
	assert_jump(true, { 1, 3, 7 });

	assert_jump(false, { 2, 0, 0 });
	assert_jump(false, { 0, 4, 0 });
	assert_jump(false, { 0, 0, 8 });
	assert_jump(false, { 2, 4, 8 });
}

P3_UNIT_TEST(grid_pos_next_prev)
{
	p3::grid_pos<2> pos{ {7, 3} };
	pos.jump({ 3, 2 });

	const auto copy1 = pos;
	pos.next();
	const auto copy2 = pos;
	pos.prev();
	const auto copy3 = pos;

	if (copy1 != copy3)
	{
		unit_test::assert_equals(0, 1, "grid_pos: next(); prev(); copies are not equal");
	}

	unit_test::assert_equals<size_t>(copy1.index() + 1, copy2.index(), "grid_pos::index() is not higher than the old value");
	unit_test::assert_equals<size_t>(copy1.index(), copy3.index(), "grid_pos::index() did not return to the old value");
}

P3_UNIT_TEST(grid_pos_index)
{
	constexpr p3::grid_size<4> size{ 7, 3, 5, 2 };
	constexpr auto elements = size.elements();

	size_t index = 0;
	for (p3::grid_pos<4> pos{ size }; pos.valid(); ++pos, ++index)
	{
		unit_test::assert_equals<size_t>(index, pos.index(), "grid_pos::index()");
	}
	unit_test::assert_equals<size_t>(elements, index, "grid_pos final count of iterations");
}

P3_UNIT_TEST(grid_pos_overflow)
{
	p3::grid_pos<3> pos{ {2, 2, 2} };
	unit_test::assert_equals<bool>(true, pos.valid(), "grid_pos::valid()");
	pos.last();
	unit_test::assert_equals<bool>(true, pos.valid(), "grid_pos::valid()");
	pos.next();
	unit_test::assert_equals<bool>(false, pos.valid(), "grid_pos::valid()");
	pos.first();
	unit_test::assert_equals<bool>(true, pos.valid(), "grid_pos::valid()");
}

#pragma endregion
#pragma region grid

P3_UNIT_TEST(grid_constructor_default)
{
	const p3::grid<int, 3> g = {};
}

P3_UNIT_TEST(grid_constructor_size)
{
	constexpr p3::grid_size<3> size{ 2, 3, 2 };
	const p3::grid<int, 3> grid(size);

	unit_test::assert_equals<size_t>(3, grid.rank(), "grid::rank()");

	for (size_t i = 0; i < size.size(); ++i)
	{
		unit_test::assert_equals<size_t>(size[i], grid.dim()[i],  std::format("grid::dim()[{0}]", i));
		unit_test::assert_equals<size_t>(size[i], grid.dim_at(i), std::format("grid::dim_at({0})", i));
	}
}

P3_UNIT_TEST(grid_constructor_size_complete_list)
{
	const p3::grid<unsigned, 2> grid{ {3, 3}, {1, 2, 3, 4, 5, 6, 7, 8, 9} };
	
	size_t index = 1;
	for (size_t y = 0; y < grid.dim_at(1); ++y)
	{
		for (size_t x = 0; x < grid.dim_at(0); ++x)
		{
			unit_test::assert_equals(index, grid.at({y, x}), std::format("grid::at({{{0}, {1}}})", y, x));
			++index;
		}
	}
}

P3_UNIT_TEST(grid_constructor_size_incomplete_list)
{
	const p3::grid<unsigned, 3> grid{ { 2, 2, 2 }, { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 } };
	unit_test::assert_equals<size_t>(4, grid.dim_at(0), "grid::dim_at(0)");

	const size_t incrementing_until = 13;
	for (size_t i = 0; i < incrementing_until; ++i)
	{
		unit_test::assert_equals<size_t>(i+1, grid[i], "value initialized part");
	}
	for (size_t i = incrementing_until; i < grid.size(); ++i)
	{
		unit_test::assert_equals<size_t>(0, grid[i], "zero-initialized part");
	}
}

P3_UNIT_TEST(grid_constructor_iterator)
{
	const std::string text = "123456789";
	const p3::grid<char, 2> grid({ 0, 3 }, text.begin(), text.end());

	unit_test::assert_equals<size_t>(3, grid.dim_at(0), "grid: iterator constructor: dimensions did not grow to fit iterator area");
	unit_test::assert_equals<size_t>(text.size(), grid.size(), "grid: iterator constructor: element count does not match");

	// hmm, doesn't compile for some reason...
	// unit_test::assert_equals(text.begin(), text.end(), grid.begin(), grid.end(), "grid: iterator constructor did not correctly copy the elements");

	auto text_iter = text.begin();
	auto grid_iter = grid.begin();

	for (; text_iter != text.end() && grid_iter != grid.end(); ++text_iter, ++grid_iter)
	{
		unit_test::assert_equals<char>(*text_iter, *grid_iter, "grid: iterator constructor: data copied incorrectly");
	}
}

P3_UNIT_TEST(grid_constructor_generator)
{
	const auto operation = [](auto a, auto b) { return 2 * a + b; };

	constexpr p3::grid_size<2> size{ 10, 10 };
	const p3::grid<int, 2> grid{ size, [&](const auto &pos) { return operation(pos.pos_at(0), pos.pos_at(1)); } };
	
	for (size_t i = 0; i < grid.rank(); ++i)
	{
		unit_test::assert_equals<size_t>(size[i], grid.dim_at(i));
	}

	for (size_t y = 0; y < grid.dim_at(0); ++y)
	{
		for (size_t x = 0; x < grid.dim_at(1); ++x)
		{
			const auto expected = operation(y, x);
			const auto actual = grid.at({y, x});
			unit_test::assert_equals<int>(expected, actual, std::format("grid::at({{{0}, {1}}})", x, y));
		}
	}
}

P3_UNIT_TEST(grid_constructor_endless_loop)
{
	// so the idea of this test was initially to make an endless loop, by writing to the given position object.
	// It was supposed to fail whenever the position index wasn't fitting anymore.
	// I did not explicitly program it like that, but I was pleasantly surprised when the program didn't compile with a non-const reference.

	// my fix to that would have been to add a const ref in the constructor (aliasing my position), and passing that one through.
	// the alternative of just stupidly copying the position for every entry (even for const ref functions) was immediately off the table.

	constexpr p3::grid_size<4> size{ 6, 3, 5, 8 };
	constexpr auto elements = size.elements();

	size_t index = 0;
	const auto generator = [&](const auto &pos) // [&](auto &pos)
	{
		// pos.pos() = {};
		unit_test::assert_equals(index, pos.index(), "position index mismatch");
		return index++;
	};

	const p3::grid<size_t, 4> obj(size, generator);
}

P3_UNIT_TEST(grid_constructor_converter)
{
	constexpr p3::grid_size<3> size{ 3, 4, 5 };
	constexpr auto elements = size.elements();
	const auto generator = [](const p3::grid_pos<3> &pos) { return pos.pos_at(0) * pos.pos_at(1) + pos.pos_at(2); };
	const auto converter = [](const p3::grid_pos<3> &pos, const auto &old) { return old - 10; };

	const p3::grid<int, 3> generated{ size, generator };
	const p3::grid<float, 3> copy = generated;
	const p3::grid<int, 3> converted{ generated, converter };

	for (size_t i = 0; i < converted.rank(); ++i)
	{
		unit_test::assert_equals<size_t>(size[i], converted.dim_at(i));
	}

	// still no iterators implemented, sorry...
	for (size_t i = 0; i < elements; ++i)
	{
		const auto expected = generated[i] - 10;
		const auto actual = converted[i];
		unit_test::assert_equals<int>(expected, actual, std::format("grid::operator[]({0})", i));
	}
}

P3_UNIT_TEST(grid_gen_ascending)
{
	const p3::grid<int, 3> expected({ 5, 2, 3 }, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29 });
	const p3::grid<int, 3> actual(expected.dim(), p3::grid_gen::ascending<3>);
	grid_test::assert_equal_grids(expected, actual, "grid_gen::ascending<3>()");
}

P3_UNIT_TEST(grid_gen_decimal_unrestricted)
{
	const p3::grid<int, 2> expected({ 7, 6 },
	{
		00, 01, 02, 03, 04, 05,
		10, 11, 12, 13, 14, 15,
		20, 21, 22, 23, 24, 25,
		30, 31, 32, 33, 34, 35,
		40, 41, 42, 43, 44, 45,
		50, 51, 52, 53, 54, 55,
		60, 61, 62, 63, 64, 65
	});
	const p3::grid<int, 2> actual(expected.dim(), p3::grid_gen::positional_notation<int>(10));
	grid_test::assert_equal_grids(expected, actual, "grid_gen::positional_notation(10)");
}

P3_UNIT_TEST(grid_gen_binary_operators)
{
	const p3::grid<int, 2> expected_add({ 5, 5 },
	{
		 0,  1,  2,  3,  4,  
		 1,  2,  3,  4,  5,  
		 2,  3,  4,  5,  6,  
		 3,  4,  5,  6,  7,  
		 4,  5,  6,  7,  8
	});

	const p3::grid<int, 2> expected_mul({11, 11},
	{
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,
		0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
		0,  2,  4,  6,  8, 10, 12, 14, 16, 18,  20,
		0,  3,  6,  9, 12, 15, 18, 21, 24, 27,  30,
		0,  4,  8, 12, 16, 20, 24, 28, 32, 36,  40,
		0,  5, 10, 15, 20, 25, 30, 35, 40, 45,  50,
		0,  6, 12, 18, 24, 30, 36, 42, 48, 54,  60,
		0,  7, 14, 21, 28, 35, 42, 49, 56, 63,  70,
		0,  8, 16, 24, 32, 40, 48, 56, 64, 72,  80,
		0,  9, 18, 27, 36, 45, 54, 63, 72, 81,  90,
		0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100
	});

	const p3::grid<int, 2> expected_xor({ 8, 8 }, 
	{
		0, 1, 2, 3, 4, 5, 6, 7,
		1, 0, 3, 2, 5, 4, 7, 6,
		2, 3, 0, 1, 6, 7, 4, 5,
		3, 2, 1, 0, 7, 6, 5, 4,
		4, 5, 6, 7, 0, 1, 2, 3,
		5, 4, 7, 6, 1, 0, 3, 2,
		6, 7, 4, 5, 2, 3, 0, 1,
		7, 6, 5, 4, 3, 2, 1, 0
	});

	const p3::grid<int, 2> actual_add(expected_add.dim(), p3::grid_gen::axis_add<int>());
	const p3::grid<int, 2> actual_mul(expected_mul.dim(), p3::grid_gen::axis_mul<int>());
	const p3::grid<int, 2> actual_xor(expected_xor.dim(), p3::grid_gen::axis_xor<int>());

	grid_test::assert_equal_grids(expected_add, actual_add, "grid_gen::axis_add()");
	grid_test::assert_equal_grids(expected_mul, actual_mul, "grid_gen::axis_mul()");
	grid_test::assert_equal_grids(expected_xor, actual_xor, "grid_gen::axis_xor()");
}

P3_UNIT_TEST(grid_value_assignment)
{
	constexpr p3::grid_size<2> position = { 1, 2 };
	constexpr char val1 = 'a';
	constexpr char val2 = 'b';

	p3::grid<char, 2> grid{ { 4, 3 } };

	const auto position_index = [&]() { return grid.dim().index_of(position); };
	const auto get_expected = [&](size_t index, char val) { return index == position_index() ? val : '\0'; };

	// sorry, iterators aren't implemented, yet.
	grid.at(position) = val1;
	for (size_t i = 0; i < grid.size(); ++i)
	{
		const auto actual = grid[i];
		const auto expected = get_expected(i, val1);
		unit_test::assert_equals<char>(expected, actual, std::format("grid::at() = {0};", val1));
	}

	grid[position_index()] = val2;
	for (size_t i = 0; i < grid.size(); ++i)
	{
		const auto actual = grid[i];
		const auto expected = get_expected(i, val2);
		unit_test::assert_equals<char>(expected, actual, std::format("grid::operator[]() = {0};", val2));
	}
}

P3_UNIT_TEST(grid_data)
{
	const p3::grid<int, 5> grid({1, 1, 1, 1, 1});
	if (&grid[0] != grid.data())
	{
		unit_test::assert_equals(0, 1, "grid::data() pointer does not match");
	}
}

P3_UNIT_TEST(grid_inside)
{
	const p3::grid<int, 4> grid({ 8, 6, 3, 4 });
	unit_test::assert_equals<bool>( true, grid.inside({ 0, 0, 0, 0 }), "grid::inside({ 0, 0, 0, 0 })");
	unit_test::assert_equals<bool>( true, grid.inside({ 0, 0, 0, 1 }), "grid::inside({ 0, 0, 0, 1 })");
	unit_test::assert_equals<bool>( true, grid.inside({ 0, 0, 1, 1 }), "grid::inside({ 0, 0, 1, 1 })");
	unit_test::assert_equals<bool>( true, grid.inside({ 0, 1, 1, 1 }), "grid::inside({ 0, 1, 1, 1 })");
	unit_test::assert_equals<bool>( true, grid.inside({ 1, 1, 1, 1 }), "grid::inside({ 1, 1, 1, 1 })");

	unit_test::assert_equals<bool>( true, grid.inside({ 7, 5, 2, 3 }), "grid::inside({ 7, 5, 2, 3 })");

	unit_test::assert_equals<bool>(false, grid.inside({ 7, 5, 2, 4 }), "grid::inside({ 7, 5, 2, 4 })");
	unit_test::assert_equals<bool>(false, grid.inside({ 7, 5, 3, 3 }), "grid::inside({ 7, 5, 3, 3 })");
	unit_test::assert_equals<bool>(false, grid.inside({ 7, 6, 2, 3 }), "grid::inside({ 7, 6, 2, 3 })");
	unit_test::assert_equals<bool>(false, grid.inside({ 8, 5, 2, 3 }), "grid::inside({ 8, 5, 2, 3 })");

	unit_test::assert_equals<bool>(false, grid.inside({ 0, 0, 0, 4 }), "grid::inside({ 0, 0, 0, 4 })");
	unit_test::assert_equals<bool>(false, grid.inside({ 0, 0, 3, 0 }), "grid::inside({ 0, 0, 3, 0 })");
	unit_test::assert_equals<bool>(false, grid.inside({ 0, 6, 0, 0 }), "grid::inside({ 0, 6, 0, 0 })");
	unit_test::assert_equals<bool>(false, grid.inside({ 8, 0, 0, 0 }), "grid::inside({ 8, 0, 0, 0 })");
}

P3_UNIT_TEST(grid_iterators)
{
	constexpr p3::grid_size<2> size{ 16, 16 };
	const p3::grid<int, 2> grid{ size, [](const auto &pos) { return pos.pos_at(0) ^ pos.pos_at(1); } };

	size_t counter = 0;
	for (const auto &elem : grid)
	{
		++counter;
	}
	unit_test::assert_equals<size_t>(size.elements(), counter, "grid: iteration count");

	auto copy = grid;
	auto grid_iter = grid.begin();
	auto copy_iter = copy.begin();
	
	for (; grid_iter != grid.end() && copy_iter != copy.end(); ++grid_iter, ++copy_iter)
	{
		unit_test::assert_equals<int>(*grid_iter, *copy_iter, "grid: dual iteration element comparison");
	}
	if (grid_iter != grid.end() || copy_iter != copy.end())
	{
		unit_test::assert_equals(1, 0, "grid: iterator count didn't match");
	}

	for (auto &elem : copy)
	{
		elem = 0;
	}
	for (const auto &elem : copy)
	{
		unit_test::assert_equals<int>(0, elem, "grid: iterator writing didn't work");
	}
}

P3_UNIT_TEST(grid_comparison)
{
	const p3::grid<int, 3> def1, def2;
	if (def1 != def1)
	{
		unit_test::assert_equals(0, 1, "grid: default object did not equal itself");
	}
	if (def1 != def2)
	{
		unit_test::assert_equals(0, 1, "grid: default objects did not achieve equality");
	}

	const std::string text = "Hello,World!";
	// hm, this begs for a grid(size, begin, end) constructor. I'll think about it.
	const p3::grid<char, 2> equ1{ {2, 6}, {'H', 'e', 'l', 'l', 'o', ',', 'W', 'o', 'r', 'l', 'd', '!'} };
	const p3::grid<char, 2> equ2{ {2, 6}, text.begin(), text.end() };

	// one entry changed
	const p3::grid<char, 2> neq1{ {2, 6}, {'H', 'e', 'l', 'l', 'o', ',', 'W', 'a', 'r', 'l', 'd', '!'} };
	// flipped dimensions
	const p3::grid<char, 2> neq2{ {6, 2}, {'H', 'e', 'l', 'l', 'o', ',', 'W', 'o', 'r', 'l', 'd', '!'} };

	if (equ1 != equ1)
	{
		unit_test::assert_equals(0, 1, "grid: filled grid did not equal itself");
	}
	if (equ1 != equ2)
	{
		unit_test::assert_equals(0, 1, "grid: equally filled grids did not achieve equality");
	}

	if (equ1 == neq1)
	{
		unit_test::assert_equals(0, 1, "grid: non-equal filled grids achieved equality");
	}
	if (equ1 == neq1)
	{
		unit_test::assert_equals(0, 1, "grid: filled grids with flipped dimensions achieved equality");
	}
}

P3_UNIT_TEST(grid_iterate)
{
	size_t iterations = 0;
	const auto  count_iterations = [&](const auto &pos, const auto &val) { ++iterations; };
	const auto generate_ascending = [](const auto &pos) { return pos.index(); };
	const auto   assert_ascending = [](const auto &pos, const auto &val) { unit_test::assert_equals<int>(pos.index(), val, "grid::iterate(): values mismatch"); };
	const auto   overwrite_zeroes = [](const auto &pos, auto &val) { val = 0; };
	const auto      assert_zeroes = [](const auto &pos, const auto &val) { unit_test::assert_equals<int>(0, val, "grid::iterate(): value is not zero."); };

	const p3::grid<int, 4> grid_const({ 3, 3, 3, 3 }, &p3::grid_gen::ascending<4>);
	grid_const.iterate(assert_ascending);
	grid_const.iterate(count_iterations);
	unit_test::assert_equals<size_t>(grid_const.size(), iterations, "grid::iterate(): iteration count mismatch");
	// grid_const.iterate(overwrite_zeroes); // <- should not compile, as it's attempting to modify a const object.

	p3::grid<int, 4> grid_mutable = grid_const;
	grid_mutable.iterate(assert_ascending);
	grid_mutable.iterate(overwrite_zeroes);
	grid_mutable.iterate(assert_zeroes);
}

P3_UNIT_TEST(grid_resize_predefined)
{
	using data_type = unsigned short;
	using grid_type = p3::grid<data_type, 2>;

	// I am aware that I'm using octal values here. as long as I don't go above 07, I'm fine.
	const grid_type original({ 5, 5 },
	{
		00, 01, 02, 03, 04,
		10, 11, 12, 13, 14,
		20, 21, 22, 23, 24,
		30, 31, 32, 33, 34,
		40, 41, 42, 43, 44
	});

	const auto copy = original;

	const grid_type minus_minus({ 4, 3 },
	{
		00, 01, 02,
		10, 11, 12,
		20, 21, 22,
		30, 31, 32
	});

	const grid_type minus_plus({ 3, 6 },
	{
		00, 01, 02, 03, 04, 00,
		10, 11, 12, 13, 14, 00,
		20, 21, 22, 23, 24, 00
	});

	const grid_type plus_minus({ 7, 2 },
	{
		00, 01,
		10, 11,
		20, 21,
		30, 31, 
		40, 41,
		00, 00,
		00, 00
	});

	const grid_type plus_plus({ 7, 8 },
	{
		00, 01, 02, 03, 04, 00, 00, 00, 
		10, 11, 12, 13, 14, 00, 00, 00,
		20, 21, 22, 23, 24, 00, 00, 00,
		30, 31, 32, 33, 34, 00, 00, 00,
		40, 41, 42, 43, 44, 00, 00, 00,
		00, 00, 00, 00, 00, 00, 00, 00,
		00, 00, 00, 00, 00, 00, 00, 00
	});

	const auto compare = [&](const grid_type &actual, const std::string &name)
	{
		const grid_type empty(actual.dim());
		auto keep = original;
		auto discard = original;

		// keep data
		keep.resize(actual.dim(), true);
		grid_test::assert_equal_grids<data_type>(keep, actual, name + " (keep)");

		// discard data
		discard.resize(actual.dim());
		grid_test::assert_equal_grids<data_type>(empty, discard, name + " (discard)");
	};

	compare(copy, "copy");
	compare(minus_minus, "minus_minus");
	compare(minus_plus,  "minus_plus");
	compare(plus_minus,  "plus_minus");
	compare(plus_plus,   "plus_plus");
}

P3_UNIT_TEST(grid_subgrid_3d_small)
{
	const p3::grid<int, 3> grid({ 2, 3, 4 }, &p3::grid_gen::ascending<3>);  //  0  1  2 ...  9 10 11
	const std::vector<p3::grid<int, 2>> axis_z // 2 possible subgrids in the first axis
	{
		p3::grid<int, 2>({ 3, 4 }, {  0,  1,  2,  3,    4,  5,  6,  7,    8,  9, 10, 11 }),
		p3::grid<int, 2>({ 3, 4 }, { 12, 13, 14, 15,   16, 17, 18, 19,   20, 21, 22, 23 })
	};
	const std::vector<p3::grid<int, 2>> axis_y // 3 possible subgrids in the second axis
	{
		p3::grid<int, 2>({ 2, 4 }, { 0, 1,  2,  3,   12, 13, 14, 15 }),
		p3::grid<int, 2>({ 2, 4 }, { 4, 5,  6,  7,   16, 17, 18, 19 }),
		p3::grid<int, 2>({ 2, 4 }, { 8, 9, 10, 11,   20, 21, 22, 23 })
	};
	const std::vector<p3::grid<int, 2>> axis_x // 4 possible subgrids in the third axis
	{
		p3::grid<int, 2>({ 2, 3 }, { 0, 4,  8,   12, 16, 20 }),
		p3::grid<int, 2>({ 2, 3 }, { 1, 5,  9,   13, 17, 21 }),
		p3::grid<int, 2>({ 2, 3 }, { 2, 6, 10,   14, 18, 22 }),
		p3::grid<int, 2>({ 2, 3 }, { 3, 7, 11,   15, 19, 23 })
	};

	grid_test::assert_subgrid_axis(grid, axis_z, 0);
	grid_test::assert_subgrid_axis(grid, axis_y, 1);
	grid_test::assert_subgrid_axis(grid, axis_x, 2);
}

P3_UNIT_TEST(grid_subgrid_3d_large)
{
	const p3::grid<int, 3> grid({ 3, 4, 5 }, &p3::grid_gen::ascending<3>);  //  0  1  2 ... 57 58 59
	const std::vector<p3::grid<int, 2>> axis_z // 3 possible subgrids in the first axis
	{
		p3::grid<int, 2>({ 4, 5 }, {  0,  1,  2,  3,  4,    5,  6,  7,  8,  9,   10, 11, 12, 13, 14,   15, 16, 17, 18, 19 }),
		p3::grid<int, 2>({ 4, 5 }, { 20, 21, 22, 23, 24,   25, 26, 27, 28, 29,   30, 31, 32, 33, 34,   35, 36, 37, 38, 39 }),
		p3::grid<int, 2>({ 4, 5 }, { 40, 41, 42, 43, 44,   45, 46, 47, 48, 49,   50, 51, 52, 53, 54,   55, 56, 57, 58, 59 })
	};
	const std::vector<p3::grid<int, 2>> axis_y // 4 possible subgrids in the second axis
	{
		p3::grid<int, 2>({ 3, 5 }, {  0,  1,  2,  3,  4,   20, 21, 22, 23, 24,   40, 41, 42, 43, 44 }),
		p3::grid<int, 2>({ 3, 5 }, {  5,  6,  7,  8,  9,   25, 26, 27, 28, 29,   45, 46, 47, 48, 49 }),
		p3::grid<int, 2>({ 3, 5 }, { 10, 11, 12, 13, 14,   30, 31, 32, 33, 34,   50, 51, 52, 53, 54 }),
		p3::grid<int, 2>({ 3, 5 }, { 15, 16, 17, 18, 19,   35, 36, 37, 38, 39,   55, 56, 57, 58, 59 })
	};
	const std::vector<p3::grid<int, 2>> axis_x // 5 possible subgrids in the third axis
	{
		p3::grid<int, 2>({ 3, 4 }, { 0, 5, 10, 15,   20, 25, 30, 35,   40, 45, 50, 55 }),
		p3::grid<int, 2>({ 3, 4 }, { 1, 6, 11, 16,   21, 26, 31, 36,   41, 46, 51, 56 }),
		p3::grid<int, 2>({ 3, 4 }, { 2, 7, 12, 17,   22, 27, 32, 37,   42, 47, 52, 57 }),
		p3::grid<int, 2>({ 3, 4 }, { 3, 8, 13, 18,   23, 28, 33, 38,   43, 48, 53, 58 }),
		p3::grid<int, 2>({ 3, 4 }, { 4, 9, 14, 19,   24, 29, 34, 39,   44, 49, 54, 59 })
	};

	grid_test::assert_subgrid_axis(grid, axis_z, 0);
	grid_test::assert_subgrid_axis(grid, axis_y, 1);
	grid_test::assert_subgrid_axis(grid, axis_x, 2);
}

P3_UNIT_TEST(grid_subgrid_4d_tiny)
{
	const p3::grid<int, 4> grid({ 2, 2, 2, 2 }, &p3::grid_gen::ascending<4>);

	const std::vector<std::vector<p3::grid<int, 3>>> data
	{
		{ // axis 0
			p3::grid<int, 3>({ 2, 2, 2 }, { 0, 1,  2,  3,  4,  5,  6,  7 }),
			p3::grid<int, 3>({ 2, 2, 2 }, { 8, 9, 10, 11, 12, 13, 14, 15 })
		},
		{ // axis 1
			p3::grid<int, 3>({ 2, 2, 2 }, { 0, 1, 2, 3,  8,  9, 10, 11 }),
			p3::grid<int, 3>({ 2, 2, 2 }, { 4, 5, 6, 7, 12, 13, 14, 15 })
		},
		{ // axis 2
			p3::grid<int, 3>({ 2, 2, 2 }, { 0, 1, 4, 5,  8,  9, 12, 13 }),
			p3::grid<int, 3>({ 2, 2, 2 }, { 2, 3, 6, 7, 10, 11, 14, 15})
		},
		{ // axis 3
			p3::grid<int, 3>({ 2, 2, 2 }, { 0, 2, 4, 6, 8, 10, 12, 14 }),
			p3::grid<int, 3>({ 2, 2, 2 }, { 1, 3, 5, 7, 9, 11, 13, 15 })
		}
	};

	// size: { 2, 2, 2, 2 } -> { *, 2, 2, 2 } -> { 2, 2, 2 }
	//  -> layer 0 : diff = { 0, 1, 1, 1, 1, 1, 1, 1 }
	//  -> layer 1 : diff = { 8, 1, 1, 1, 1, 1, 1, 1 }
	//  -> period = 8, delta = 8

	// size: { 2, 2, 2, 2 } -> { 2, *, 2, 2 } -> { 2, 2, 2 }
	//  -> layer 0 : diff = { 0, 1, 1, 1, 5, 1, 1, 1 }
	//  -> layer 1 : diff = { 4, 1, 1, 1, 5, 1, 1, 1 }
	//  -> period = 4, delta = 4

	// size: { 2, 2, 2, 2 } -> { 2, 2, *, 2 } -> { 2, 2, 2 }
	//  -> layer 0 : diff = { 0, 1, 3, 1, 3, 1, 3, 1 }
	//  -> layer 1 : diff = { 2, 1, 3, 1, 3, 1, 3, 1 }
	//  -> period = 2, delta = 2

	// size: { 2, 2, 2, 2 } -> { 2, 2, 2, * } -> { 2, 2, 2 }
	//  -> layer 0 : diff = { 0, 2, 2, 2, 2, 2, 2, 2 }
	//  -> layer 1 : diff = { 1, 2, 2, 2, 2, 2, 2, 2 }
	//  -> period = 1, delta = 1

	for (size_t i = 0; i < data.size(); ++i)
	{
		grid_test::assert_subgrid_axis(grid, data[i], i);
	}
}

P3_UNIT_TEST(grid_slice)
{
	const p3::grid<int, 3> grid({ 3, 4, 5 }, &p3::grid_gen::ascending<3>);

	// slice through every axis and compare each slice with a subgrid.
	for (size_t axis = 0; axis < grid.rank(); ++axis)
	{
		const auto slices = grid.slice(axis);
		for (size_t layer = 0; layer < slices.size(); ++layer)
		{
			const auto &slice = slices[layer];
			const auto subgrid = grid.subgrid(layer, axis);

			if (slice != subgrid)
			{
				unit_test::assert_equals(0, 1, std::format("grid::subgrid({0}, {1}) did not equal grid::slice({1})[{0}]", layer, axis));
			}
		}
	}
}

#pragma endregion