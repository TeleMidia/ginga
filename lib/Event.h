/* Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia

This file is part of Ginga (Ginga-NCL).

Ginga is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Ginga is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.  */

#ifndef GINGA_EVENT_H
#define GINGA_EVENT_H

#include "aux-ginga.h"
#include "Predicate.h"

GINGA_NAMESPACE_BEGIN

class Object;

/**
 * @brief Event in an NCL object.
 *
 * Run-time representation of an NCL event state machine.
 *
 * @see Object.
 */
class Event
{
public:

  /**
   * @brief Possible types for NCL events.
   */
  enum Type
  {
     /**
      * @brief Stands for the attribution of a value to a property of the
      * object.
      */
     ATTRIBUTION = 1 << 1,

     /**
      * @brief Stands for the presentation of a time interval of the object.
      */
     PRESENTATION = 1 << 2,

     /**
      * @brief Stands for a key press/release directed to the object.
      */
     SELECTION = 1 << 3,
  };

  /**
   * @brief Converts Event::Type to a human-readable string.
   * @param type The Event::Type to convert.
   * @return The resulting string.
   */
  static string getTypeAsString (Event::Type type);

  /**
   * @brief Possible states for NCL events.
   */
  enum State
  {
     OCCURRING,                 ///< The event is occurring.
     PAUSED,                    ///< The event is paused.
     SLEEPING,                  ///< The event is sleeping.
  };

  /**
   * @brief Converts Event::State to a human-readable string.
   * @param state The Event::State to convert.
   * @return The resulting string.
   */
  static string getStateAsString (Event::State state);

  /**
   * @brief Possible transitions between states of an NCL event.
   */
  enum Transition
  {
     ABORT,                 ///< Occurring|Paused->Sleeping.
     PAUSE,                 ///< Occurring->Paused.
     RESUME,                ///< Paused->Occurring.
     START,                 ///< Paused|Sleeping->Occurring.
     STOP,                  ///< Occurring|Paused->Sleeping.
  };

  /**
   * @brief Converts Event::Transition to a human-readable string
   * @param trans The Event::Transition to convert.
   * @return The resulting string.
   */
  static string getTransitionAsString (Event::Transition trans);

  /**
   * @brief Creates a new event.
   * @param type The type of the new event.
   * @param object The container object.
   * @param id The id of the new event (must no occur in \p object).
   */
  Event (Event::Type type, Object *object, const string &id);

  /**
   * @brief Destroys the event.
   */
  ~Event ();

  /**
   * @brief Gets a string representation of event.
   * @return A string representation of event.
   */
  string toString ();

  /**
   * @brief Gets the type of event.
   * @return The type of event.
   */
  Event::Type getType ();

  /**
   * @brief Gets the container object of event.
   * @return The container object of event.
   */
  Object *getObject ();

  /**
   * @brief Gets the id of event.
   * @return The id of event.
   */
  string getId ();

  /**
   * @brief Gets the current state of event.
   * @return The current state of event.
   */
  Event::State getState ();

  // TODO ------------------------------------------------------------------

  string getFullId ();

  bool isLambda ();
  void getInterval (Time *, Time *);
  void setInterval (Time, Time);

  bool hasLabel ();
  std::string getLabel ();
  void setLabel (const std::string &);

  bool getParameter (const string &, string *);
  bool setParameter (const string &, const string &);

  bool transition (Event::Transition);
  void reset ();

public:
  static Event::Transition getStringAsTransition (string str);

private:
  /// The type of this event.
  Event::Type _type;

  /// The container object of this event.
  Object *_object;

  /// The Lua state associated with the container object.
  lua_State *_L;

  /// The id of this event.
  string _id;

  /// The current state of this event.
  Event::State _state;

  /// The start time of this event.
  Time _begin;

  /// The end time of this event.
  Time _end;

  /// The label associated with this event (if any).
  std::string _label;

  /// Extra parameters to event transitions.
  map<string, string> _parameters;
};

/**
 * @brief Action.
 */
typedef struct
{
  Event *event;                 ///< Target event.
  Event::Transition transition; ///< Desired transition.
  Predicate *predicate;         ///< Predicate conditioning the execution.
  string value;                 ///< Value to set (if attribution).
  string duration;              ///< Duration.
  string delay;                 ///< Delay.
} Action;

GINGA_NAMESPACE_END

#endif // GINGA_EVENT_H
