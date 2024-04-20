/*
 * lux - a lightweight unix-like operating system
 * Omar Elghoul, 2024
 * 
 * User-level implementation of the lux file system
 */

#pragma once

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstddef>      // size_t
#include <cstdint>      // integers
#include <string>

using namespace std;

int create(int, char **);
