# CurrencyChecker

## Overview
This tool installs and manages a service that periodically fetches and saves currency exchange rates based on your preferences.

### Interactive menu
Installation, uninstallation and control can also be done via interactive menu that spawns if you run the executable without any arguments:
```
nbu_checker.exe
```
This will spawn a new console window with a prompt.

## Installation and Uninstallation

### Installation
To install the service, run the executable with the `--install` flag from the command prompt:
```
nbu_checker.exe --install
```
This will install the service and start it immediately. Ensure that the executable path and the service name are correctly specified in the code.

### Uninstallation
To uninstall the service, use the `--uninstall` flag:
```
nbu_checker.exe --uninstall
```
This will stop the service if it is running and remove it from the system.

## Configuration

### Customizing Configuration
The service configuration is managed through a JSON file (`config.json`) in **the same directory as the executable**, which is loaded at startup.

To modify the configuration, edit the `config.json` file with appropriate values. If the configuration file does not exist, the system will generate a default configuration which should be customized before proceeding.

### Configuration Options
Below is an explanation of each field in the `config.json` file, which is crucial for the proper operation of the service:

- `dataFilePath`: 
  - **Type**: String
  - **Description**: Specifies the absolute path to the JSON data file that the application uses. 
  - **Example**: `"C:\\Users\\every\\Desktop\\uni\\job\\nbu_checker\\x64\\Debug\\data.json"`
  - **Note**: Ensure that the path is accessible and has the necessary read/write permissions.

- `dataFormat`: 
  - **Type**: String
  - **Description**: Defines the format of the data stored in the file specified by `dataFilePath`.
  - **Allowed Values**: `"JSON", "XML" or "CSV"

- `debug`: 
  - **Type**: Boolean
  - **Description**: Toggles the debug mode.
  - **Allowed Values**: `true` or `false`
  - **Default**: `true`

- `interval`: 
  - **Type**: Integer
  - **Description**: Specifies the interval, in milliseconds, at which the data file is updated
  - **Example**: `20000` (which equates to 20 seconds)
  - **Note**: Must be at least 5000(5 seconds) to prevent spam

### Example configuration file:

```
{
    "dataFilePath": "C:\\Users\\every\\Desktop\\uni\\job\\nbu_checker\\x64\\Debug\\data.json",
    "dataFormat": "JSON",
    "debug": true,
    "interval": 20000
}
```

## Dependencies

This service depends on the following external libraries:
- [`httplib`](https://github.com/yhirose/cpp-httplib): Used for performing HTTPS requests to the API.
- [`openssl`](https://www.openssl.org/): Dependency of httplib to allow SSL connections.
- [`Json for Modern C++`](https://github.com/nlohmann/json): Used for parsing and validating JSON.
- [`tinyxml2`](https://github.com/leethomason/tinyxml2): Used for validating XML.

## Usage

### Control Codes
The system provides multiple control commands to manage the service through the console:
- `--start`: Starts the service.
- `--stop`: Stops the service.
- `--pause`: Pauses the service.
- `--continue`: Resumes the service.
- `--update`: Sends a custom update command to the service.

To use a control command, run the executable with the desired flag from the command prompt:
```
nbu_checker.exe --command
```
Example:
```
nbu_checker.exe --start
```
This will start the service if it's already installed.

## Troubleshooting

### Common Issues
- **Service fails to start**: Ensure the service is correctly installed and the configuration file is correctly set up and valid.
- **Configuration issues**: If the service reports invalid configuration, verify the `config.json` contents and structure.
- **Permission issues**: Running the executable requires administrative privileges, especially for installation and uninstallation.
