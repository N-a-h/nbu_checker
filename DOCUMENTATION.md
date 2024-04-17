# Project Structure

This document outlines the structure of the project, including directories, files, and their purposes.

## Directories

### 1. `external/`
   - Contains the external dependencies of the project.

### 2. `nbu_api/`
    - Contains functionality related to fetching/validating/formatting data.
    - Files:
        - `api.hpp` - header with all the functions

### 3. `service/`
    - Contains the service implementation and service management functions
    - Files:
        - `service.hpp` - function declarations
        - `service.cpp` - implements the Windows service itself, acting as the entry point
        - `manager.hpp` - function declarations
        - `manager.cpp` - implements service management functions((un)installation/sending commands etc)
        - `logger.hpp` - implements logging
        - `logger.cpp` - declares a global variable

### Files:
- `main.cpp` - program entry point, acts as the user interface
- `config.hpp` - implements a configuration class, also holds most header imports
- `config.cpp` - declares a global config variable
- `utils.hpp` - inlined helper functions
- `README.md` - description of the project