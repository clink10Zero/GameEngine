#pragma once

#include <map>
#include <vector>
#include <string>

namespace setting
{
    enum Key
    {
        // From glfw3.h
        Space = 32,
        Apostrophe = 39, /* ' */
        Comma = 44, /* , */
        Minus = 45, /* - */
        Period = 46, /* . */
        Slash = 47, /* / */

        D0 = 48, /* 0 */
        D1 = 49, /* 1 */
        D2 = 50, /* 2 */
        D3 = 51, /* 3 */
        D4 = 52, /* 4 */
        D5 = 53, /* 5 */
        D6 = 54, /* 6 */
        D7 = 55, /* 7 */
        D8 = 56, /* 8 */
        D9 = 57, /* 9 */

        Semicolon = 59, /* ; */
        Equal = 61, /* = */

        A = 65,
        B = 66,
        C = 67,
        D = 68,
        E = 69,
        F = 70,
        G = 71,
        H = 72,
        I = 73,
        J = 74,
        K = 75,
        L = 76,
        M = 77,
        N = 78,
        O = 79,
        P = 80,
        Q = 81,
        R = 82,
        S = 83,
        T = 84,
        U = 85,
        V = 86,
        W = 87,
        X = 88,
        Y = 89,
        Z = 90,

        LeftBracket = 91,  /* [ */
        Backslash = 92,  /* \ */
        RightBracket = 93,  /* ] */
        GraveAccent = 96,  /* ` */

        World1 = 161, /* non-US #1 */
        World2 = 162, /* non-US #2 */

        /* Function keys */
        Escape = 256,
        Enter = 257,
        Tab = 258,
        Backspace = 259,
        Insert = 260,
        Delete = 261,
        Right = 262,
        Left = 263,
        Down = 264,
        Up = 265,
        PageUp = 266,
        PageDown = 267,
        Home = 268,
        End = 269,
        CapsLock = 280,
        ScrollLock = 281,
        NumLock = 282,
        PrintScreen = 283,
        Pause = 284,
        F1 = 290,
        F2 = 291,
        F3 = 292,
        F4 = 293,
        F5 = 294,
        F6 = 295,
        F7 = 296,
        F8 = 297,
        F9 = 298,
        F10 = 299,
        F11 = 300,
        F12 = 301,
        F13 = 302,
        F14 = 303,
        F15 = 304,
        F16 = 305,
        F17 = 306,
        F18 = 307,
        F19 = 308,
        F20 = 309,
        F21 = 310,
        F22 = 311,
        F23 = 312,
        F24 = 313,
        F25 = 314,

        /* Keypad */
        KP0 = 320,
        KP1 = 321,
        KP2 = 322,
        KP3 = 323,
        KP4 = 324,
        KP5 = 325,
        KP6 = 326,
        KP7 = 327,
        KP8 = 328,
        KP9 = 329,
        KPDecimal = 330,
        KPDivide = 331,
        KPMultiply = 332,
        KPSubtract = 333,
        KPAdd = 334,
        KPEnter = 335,
        KPEqual = 336,

        LeftShift = 340,
        LeftControl = 341,
        LeftAlt = 342,
        LeftSuper = 343,
        RightShift = 344,
        RightControl = 345,
        RightAlt = 346,
        RightSuper = 347,
        Menu = 348
    };

    class EditorKeyMode
    {
        public:
            Key forward = Z;
            Key backward = S;
            Key left = Q;
            Key right = D;
            Key up = A;
            Key down = E;
            Key rotationLeft = Left;
            Key rotationRight = Right;
            Key rotationUp = Up;
            Key rotationDown = Down;
    };

    class GameControle
    {
        public:
            GameControle() = default;
            ~GameControle() = default;

            void addElement(std::string name, Key value)
            {
                controle.insert(std::pair < std::string, Key >{name, value});
            }

            void remove(std::string key)
            {
                controle.erase(key);
            }

            Key getKey(std::string key)
            {
                return controle[key];
            }
            std::map<std::string, Key> getData()
            {
                return controle;
            }
        private:
            std::map<std::string, Key> controle{};
    };

    class ProjectSetting
    {
        public:
            ProjectSetting() = default;
            ~ProjectSetting() = default;

            GameControle getGameControle()
            {
                return gControle;
            }

            EditorKeyMode getEditorControle()
            {
                return eControle;
            }

            std::vector<Key> getKeys()
            {
                return allKey;
            }

        private:
            GameControle gControle{};
            EditorKeyMode eControle{};

            std::vector<Key> allKey = {
                Space,
                Apostrophe, /* ' */
                Comma, /* , */
                Minus, /* - */
                Period, /* . */
                Slash, /* / */

                D0, /* 0 */
                D1, /* 1 */
                D2, /* 2 */
                D3, /* 3 */
                D4, /* 4 */
                D5, /* 5 */
                D6, /* 6 */
                D7, /* 7 */
                D8, /* 8 */
                D9, /* 9 */

                Semicolon, /* ; */
                Equal, /* = */

                A,
                B,
                C,
                D,
                E,
                F,
                G,
                H,
                I,
                J,
                K,
                L,
                M,
                N,
                O,
                P,
                Q,
                R,
                S,
                T,
                U,
                V,
                W,
                X,
                Y,
                Z,

                LeftBracket,  /* [ */
                Backslash,  /* \ */
                RightBracket,  /* ] */
                GraveAccent,  /* ` */

                World1, /* non-US #1 */
                World2, /* non-US #2 */

                /* Function keys */
                Escape,
                Enter,
                Tab,
                Backspace,
                Insert,
                Delete,
                Right,
                Left,
                Down,
                Up,
                PageUp,
                PageDown,
                Home,
                End,
                CapsLock,
                ScrollLock,
                NumLock,
                PrintScreen,
                Pause,
                F1,
                F2,
                F3,
                F4,
                F5,
                F6,
                F7,
                F8,
                F9,
                F10,
                F11,
                F12,
                F13,
                F14,
                F15,
                F16,
                F17,
                F18,
                F19,
                F20,
                F21,
                F22,
                F23,
                F24,
                F25,

                /* Keypad */
                KP0,
                KP1,
                KP2,
                KP3,
                KP4,
                KP5,
                KP6,
                KP7,
                KP8,
                KP9,
                KPDecimal,
                KPDivide,
                KPMultiply,
                KPSubtract,
                KPAdd,
                KPEnter,
                KPEqual,

                LeftShift,
                LeftControl,
                LeftAlt,
                LeftSuper,
                RightShift,
                RightControl,
                RightAlt,
                RightSuper,
                Menu
            };
    };
};
