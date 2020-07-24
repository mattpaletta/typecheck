#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

// Include pieces of the test
#include "utils.hpp"

#ifdef TEST_RESOLVERS
#include "test_resolvers.hpp"
#endif

#ifdef TEST_TYPE_MANAGER
#include "test_type_manager.hpp"
#endif

#ifdef TEST_TYPE_CONSTRAINTS
#include "test_constraints.hpp"
#endif
