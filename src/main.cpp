/*
 * Project: task
 * File: main.cpp
 * Description: 
 * Copyright (C) 2024 Joshua Holman
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: <Your Name>
 * Contact: <Your Email>
 */

#include <iostream>
#include <list>
#include <fstream>
#include "Date.hpp"
#include "Task.hpp"

#include <map>

void help();
void add(int, char *[], std::list<Task> &);
void complete(int, char *[], std::list<Task> &);
void del(int, char *[], std::list<Task> &);
void clear(std::list<Task> &);
void view(std::list<Task> &);

void saveTasksToFile(const std::list<Task> &, const std::string &);
void loadTasksFromFile(std::list<Task> &, const std::string &);

enum Command {
  ADD,
  COMPLETE,
  DELETE,
  CLEAR,
  HELP,
  VIEW,
  UNKNOWN
};

std::map<std::string, Command> commandMap = {
  {"add", ADD},
  {"complete", COMPLETE},
  {"del", DELETE},
  {"clear", CLEAR},
  {"help", HELP},
  {"view", VIEW}
};
  

int main(int argc, char *argv[]) {
  std::list<Task> tasklist;

  if (argc == 1) {
    std::cerr << "Please provide an argument. See " << argv[0] << " help for a list of commands." << std::endl;
    return 1;
  }

  if (argc > 4) {
    std::cerr << "Too many arguments. See " << argv[0] << " help for a list of commands." << std::endl;
    return 1;
  }

  std::string modifier = argv[1];
  Command command = commandMap.count(modifier) ? commandMap[modifier] : UNKNOWN;

  switch (command) {
    case ADD:
      add(argc, argv, tasklist);
        break;
    case COMPLETE:
      complete(argc, argv, tasklist);
      break;
    case CLEAR:
      clear(tasklist);
      break;
    case DELETE:
      del(argc, argv, tasklist);
      break;
    case HELP:
      help();
      break;
    case VIEW:
      view(tasklist);
      break;
    default:
      std::cerr << "Unknown command: " << modifier << std::endl;
      std::cerr << "Try " << argv[0] << " help for a list of commands." << std::endl;
      return 1;
  }

  for (auto &obj : tasklist) {
      std::cout << obj << "\n";
  }

  return 0;
}

void saveTasksToFile(const std::list<Task> &tasklist, const std::string &filename) {
  std::ofstream file(filename);
  for (const auto& task : tasklist) {
    file << task.serialize() << std::endl;
  }
  file.close();
}

void loadTasksFromFile(std::list<Task> &tasklist, const std::string &filename) {
  std::ifstream file(filename);
  std::string line;
  while (std::getline(file, line)) {
    tasklist.push_back(Task::deserialize(line));
  }
  file.close();
}

void help() {
    std::cout << "task [option] [arguments]\n"
              << "Options:\n"
              << "add [name] [due date]   - Adds a task with optional due date\n"
              << "complete [name]         - Marks a task as complete\n"
              << "del [name]              - Deletes a task, deleted all completed tasks if given no argument\n"
              << "clear                   - Deletes all tasks\n"
              << "help                    - Displays this message\n";  
}

void add(int argc, char *argv[], std::list<Task> &tasklist) {
  if (argc < 3) {
    std::cerr << "Not enough arguments for 'add' command." << std::endl;
    return;
  }

  std::string taskname = argv[2];
  std::string dateStr = (argc > 3) ? argv[3] : "";
  Date date;

  if (!dateStr.empty()) {
    try {
      Date tempDate(dateStr);
      date = tempDate;
    } 
    catch (const std::invalid_argument &e) {
      std::cerr << "Error: " << e.what() << std::endl;
      return;
    }
  }
  loadTasksFromFile(tasklist, "saveData.txt");
  tasklist.emplace_back(taskname, date);
  saveTasksToFile(tasklist, "saveData.txt");
}

void complete(int argc, char *argv[], std::list<Task> &tasklist) {
  if (argc < 3) {
    std::cerr << "Not enough arguments for 'complete' command." << std::endl;
    return;
  }

  std::string taskname = argv[2];
  loadTasksFromFile(tasklist, "saveData.txt");
  for (auto &task : tasklist) {
    if (task.getName() == taskname) {
      task.markComplete();
      break;
    }
  }
  saveTasksToFile(tasklist, "saveData.txt");
}

void del(int argc, char *argv[], std::list<Task> &tasklist) {
  loadTasksFromFile(tasklist, "saveData.txt");
  if (argc < 3) {
    tasklist.remove_if([](const Task &task) { return task.getCompletion(); });
  } else {
    std::string taskname = argv[2];
    tasklist.remove_if([&](const Task &task) { return task.getName() == taskname; });
  }
    saveTasksToFile(tasklist, "saveData.txt");
}

void clear(std::list<Task> &tasklist) {
  tasklist.clear();
  saveTasksToFile(tasklist, "saveData.txt");
}

void view(std::list<Task> &tasklist) {
  loadTasksFromFile(tasklist, "saveData.txt");
}