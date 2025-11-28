/*
 * Museum Escape - Game Class Implementation
 * CS/CE 224/272 - Fall 2025
 */

#include "Game.h"
#include "Room.h"
#include "Puzzle.h"
#include "Guard.h"
#include "Item.h"
#include <iostream>

// Constructor
Game::Game() 
    : window(sf::VideoMode({800u, 600u}), "Museum Escape - Enhanced"),
      currentState(GameState::MENU),
      deltaTime(0.0f),
      currentRoomID(1),
      activePuzzle(nullptr),
      stateText(defaultFont),
      notificationText(notificationFont),
      notificationTimer(0.0f),
      notificationColor(sf::Color::White)
{
    window.setFramerateLimit(60);
    initialize();
}

// Destructor
Game::~Game() {
    // Cleanup handled by smart pointers
}

// Initialize game components
void Game::initialize() {
    // Load assets FIRST
    loadAssets();
    
    // Create player (Pass the texture)
    player = std::make_unique<Player>(100.0f, 100.0f, playerTexture);
    
    // Create timer (10 minutes = 600 seconds)
    gameTimer = std::make_unique<Timer>(600.0f);
    gameTimer->setDisplayPosition(650.0f, 20.0f);
    gameTimer->setFont(mainFont);
    
    // Create inventory (increased capacity)
    inventory = std::make_unique<Inventory>(15);
    inventory->setFont(mainFont);
    
    // Create rooms and puzzles
    createRooms();
    setupPuzzles();
    
    // Setup UI text
    stateText.setFont(mainFont);
    stateText.setCharacterSize(30);
    stateText.setFillColor(sf::Color::White);
    stateText.setPosition({250.0f, 250.0f});
    
    // Setup notification text
    notificationText.setFont(mainFont);
    notificationText.setCharacterSize(24);
    notificationText.setPosition({50.0f, 50.0f});
    notificationText.setOutlineThickness(2.0f);
    notificationText.setOutlineColor(sf::Color::Black);
    
    // Setup overlay
    overlay.setSize({800.0f, 600.0f});
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    
    std::cout << "\n=== MUSEUM ESCAPE - ENHANCED VERSION ===" << std::endl;
    std::cout << "Story: You're an investigative journalist breaking into" << std::endl;
    std::cout << "the museum to find evidence of illegal experiments!" << std::endl;
    std::cout << "Find the Evidence File and escape before police arrive!\n" << std::endl;
    std::cout << "Game initialized successfully!" << std::endl;
}

// Load fonts, sounds, music AND TEXTURES
void Game::loadAssets() {
    bool fontLoaded = false;
    
    // Try multiple paths for the font
    if (mainFont.openFromFile("assets/arial.ttf")) {
        fontLoaded = true;
    } else if (mainFont.openFromFile("arial.ttf")) {
        fontLoaded = true;
    } else if (mainFont.openFromFile("D:/Assignments/Sem3/OOP/Prozect/main/assets/arial.ttf")) {
        fontLoaded = true;
    }
    
    if (!fontLoaded) {
        std::cerr << "Warning: Could not load font from any path!" << std::endl;
    } else {
        std::cout << "Font loaded successfully!" << std::endl;
    }
    
    // === Load Character Sprites ===
    if (playerTexture.loadFromFile("assets/player.png")) {
        std::cout << "Loaded player texture." << std::endl;
    } else {
        std::cerr << "Failed to load assets/player.png" << std::endl;
    }

    if (guardTexture.loadFromFile("assets/guard.png")) {
        std::cout << "Loaded guard texture." << std::endl;
    } else {
        std::cerr << "Failed to load assets/guard.png" << std::endl;
    }
    
    // === Load Room Backgrounds ===
    for (int i = 1; i <= 7; ++i) {
        sf::Texture texture;
        std::string filenamePNG = "assets/room" + std::to_string(i) + ".png";
        std::string filenameJPG = "assets/room" + std::to_string(i) + ".jpg";
        
        // Try PNG first, then JPG
        if (texture.loadFromFile(filenamePNG)) {
            roomTextures[i] = texture;
            std::cout << "Loaded background: " << filenamePNG << std::endl;
        } else if (texture.loadFromFile(filenameJPG)) {
            roomTextures[i] = texture;
            std::cout << "Loaded background: " << filenameJPG << std::endl;
        } else {
            std::cerr << "Warning: Could not load background for room " << i << " (checked .png and .jpg)" << std::endl;
        }
    }
    
    std::cout << "Assets loaded!" << std::endl;
}

// Create game rooms - ENHANCED VERSION (7 rooms)
void Game::createRooms() {
    // ========================================================================
    // Room 1: Main Entrance (Tutorial)
    // ========================================================================
    auto room1 = std::make_shared<Room>(1, "Main Entrance", 0, 0, 800, 600);
    
    // Add Flashlight (REQUIRED for Room 5)
    auto flashlight = std::make_shared<Tool>("Flashlight", "flashlight", 
        "Illuminates dark areas", 150.0f, 150.0f);
    room1->addItem(flashlight);
    
    // Add Museum Map (collectible)
    auto map = std::make_shared<BasicItem>("Museum Map", 
        "Shows the layout of the museum", 650.0f, 150.0f);
    room1->addItem(map);
    
    // Add 1 guard (easy patrol)
    // CHANGED: Order is now (x, y, range, texture)
    auto guard1 = std::make_shared<Guard>(400.0f, 200.0f, 100.0f, guardTexture);
    guard1->addPatrolPoint(400.0f, 200.0f);
    guard1->addPatrolPoint(600.0f, 200.0f);
    guard1->addPatrolPoint(600.0f, 400.0f);
    guard1->addPatrolPoint(400.0f, 400.0f);
    room1->addGuard(guard1);
    
    rooms[1] = room1;
    
    // ========================================================================
    // Room 2: Ancient Artifacts Gallery (Pattern Puzzle)
    // ========================================================================
    auto room2 = std::make_shared<Room>(2, "Ancient Artifacts Gallery", 0, 0, 800, 600);
    
    // Add 1 guard (medium difficulty)
    // CHANGED: Order is now (x, y, range, texture)
    auto guard2 = std::make_shared<Guard>(400.0f, 450.0f, 110.0f, guardTexture);
    guard2->addPatrolPoint(400.0f, 450.0f);
    guard2->addPatrolPoint(400.0f, 150.0f);
    guard2->addPatrolPoint(600.0f, 150.0f);
    guard2->addPatrolPoint(600.0f, 450.0f);
    room2->addGuard(guard2);
    
    rooms[2] = room2;
    
    // ========================================================================
    // Room 3: Medieval Weapons Hall (Riddle Puzzle)
    // ========================================================================
    auto room3 = std::make_shared<Room>(3, "Medieval Weapons Hall", 0, 0, 800, 600);
    
    // Add Bolt Cutters (REQUIRED for Room 6)
    auto boltCutters = std::make_shared<Tool>("Bolt Cutters", "bolt_cutters",
        "Cuts through chains and wires", 650.0f, 500.0f);
    room3->addItem(boltCutters);
    
    // Add Red Keycard (collectible)
    auto redCard = std::make_shared<BasicItem>("Red Keycard",
        "An old security card", 150.0f, 150.0f);
    room3->addItem(redCard);
    
    // Add 1 guard
    // CHANGED: Order is now (x, y, range, texture)
    auto guard3 = std::make_shared<Guard>(400.0f, 200.0f, 100.0f, guardTexture);
    guard3->addPatrolPoint(400.0f, 200.0f);
    guard3->addPatrolPoint(600.0f, 200.0f);
    guard3->addPatrolPoint(600.0f, 450.0f);
    guard3->addPatrolPoint(400.0f, 450.0f);
    room3->addGuard(guard3);
    
    rooms[3] = room3;
    
    // ========================================================================
    // Room 4: Security Control Room (Lock Puzzle)
    // ========================================================================
    auto room4 = std::make_shared<Room>(4, "Security Control Room", 0, 0, 800, 600);
    
    // Add Access Code Note
    auto codeNote = std::make_shared<Passcode>("Access Code Note", "4738", 150.0f, 500.0f);
    room4->addItem(codeNote);
    
    // Add 2 guards (HARD)
    // CHANGED: Order is now (x, y, range, texture)
    auto guard4a = std::make_shared<Guard>(300.0f, 150.0f, 110.0f, guardTexture);
    guard4a->addPatrolPoint(300.0f, 150.0f);
    guard4a->addPatrolPoint(600.0f, 150.0f);
    room4->addGuard(guard4a);
    
    // CHANGED: Order is now (x, y, range, texture)
    auto guard4b = std::make_shared<Guard>(600.0f, 450.0f, 110.0f, guardTexture);
    guard4b->addPatrolPoint(600.0f, 450.0f);
    guard4b->addPatrolPoint(300.0f, 450.0f);
    room4->addGuard(guard4b);
    
    rooms[4] = room4;
    
    // ========================================================================
    // Room 5: Dark Archives (Math Puzzle → Green Keycard)
    // ========================================================================
    auto room5 = std::make_shared<Room>(5, "Dark Archives", 0, 0, 800, 600);
    
    // Add Encrypted Note
    auto encryptedNote = std::make_shared<BasicItem>("Encrypted Note",
        "Wire sequence: Primary colors first, then secondary", 650.0f, 150.0f);
    room5->addItem(encryptedNote);
    
    // Add 1 guard with flashlight
    // CHANGED: Order is now (x, y, range, texture)
    auto guard5 = std::make_shared<Guard>(400.0f, 400.0f, 120.0f, guardTexture);
    guard5->addPatrolPoint(400.0f, 400.0f);
    guard5->addPatrolPoint(600.0f, 400.0f);
    guard5->addPatrolPoint(600.0f, 200.0f);
    guard5->addPatrolPoint(400.0f, 200.0f);
    room5->addGuard(guard5);
    
    rooms[5] = room5;
    
    // ========================================================================
    // Room 6: Laboratory (Wire Puzzle → Master Keycard)
    // ========================================================================
    auto room6 = std::make_shared<Room>(6, "Laboratory", 0, 0, 800, 600);
    
    // Add Evidence Log
    auto evidenceLog = std::make_shared<BasicItem>("Evidence Log",
        "Documents showing illegal experiments", 150.0f, 150.0f);
    room6->addItem(evidenceLog);
    
    // Add 2 guards
    // CHANGED: Order is now (x, y, range, texture)
    auto guard6a = std::make_shared<Guard>(400.0f, 200.0f, 115.0f, guardTexture);
    guard6a->addPatrolPoint(400.0f, 200.0f);
    guard6a->addPatrolPoint(600.0f, 200.0f);
    room6->addGuard(guard6a);
    
    // CHANGED: Order is now (x, y, range, texture)
    auto guard6b = std::make_shared<Guard>(600.0f, 450.0f, 115.0f, guardTexture);
    guard6b->addPatrolPoint(600.0f, 450.0f);
    guard6b->addPatrolPoint(400.0f, 450.0f);
    room6->addGuard(guard6b);
    
    rooms[6] = room6;
    
    // ========================================================================
    // Room 7: Director's Office (FINAL ROOM)
    // ========================================================================
    auto room7 = std::make_shared<Room>(7, "Director's Office", 0, 0, 800, 600);
    
    // Add Evidence File (WIN CONDITION!)
    auto evidenceFile = std::make_shared<BasicItem>("Evidence File",
        "The proof you need to expose the conspiracy!", 400.0f, 300.0f);
    room7->addItem(evidenceFile);
    
    // Add Personal Journal
    auto journal = std::make_shared<BasicItem>("Personal Journal",
        "The Director's personal notes", 650.0f, 150.0f);
    room7->addItem(journal);
    
    // NO GUARDS - Safe haven!
    room7->setExitRoom(true);
    
    rooms[7] = room7;
    
    // ========================================================================
    // Connect Rooms
    // ========================================================================
    
    // Room 1 → Room 2
    room1->addDoor(std::make_shared<Door>(750.0f, 300.0f, 2, false, ""));
    
    // Room 2 ↔ Room 3
    room2->addDoor(std::make_shared<Door>(50.0f, 300.0f, 1, false, ""));
    room2->addDoor(std::make_shared<Door>(750.0f, 300.0f, 3, true, "blue_keycard"));
    
    // Room 3 ↔ Room 4
    room3->addDoor(std::make_shared<Door>(50.0f, 300.0f, 2, false, ""));
    room3->addDoor(std::make_shared<Door>(750.0f, 300.0f, 4, false, ""));
    
    // Room 4 ↔ Room 5
    room4->addDoor(std::make_shared<Door>(50.0f, 300.0f, 3, false, ""));
    room4->addDoor(std::make_shared<Door>(750.0f, 300.0f, 5, true, "yellow_keycard"));
    
    // Room 5 ↔ Room 6
    room5->addDoor(std::make_shared<Door>(50.0f, 300.0f, 4, false, ""));
    room5->addDoor(std::make_shared<Door>(750.0f, 300.0f, 6, true, "green_keycard"));
    
    // Room 6 ↔ Room 7
    room6->addDoor(std::make_shared<Door>(50.0f, 300.0f, 5, false, ""));
    room6->addDoor(std::make_shared<Door>(750.0f, 300.0f, 7, true, "master_keycard"));
    
    // Room 7 Back door
    room7->addDoor(std::make_shared<Door>(50.0f, 300.0f, 6, false, ""));
    
    // ========================================================================
    // APPLY TEXTURES (if loaded)
    // ========================================================================
    for (auto& pair : rooms) {
        int id = pair.first;
        if (roomTextures.find(id) != roomTextures.end()) {
            pair.second->setBackgroundTexture(roomTextures[id]);
        }
    }
    
    std::cout << "\n=== ENHANCED MUSEUM STRUCTURE ===" << std::endl;
    std::cout << "Room 1: Main Entrance (Get Flashlight!)" << std::endl;
    std::cout << "Room 2: Ancient Artifacts (Pattern Puzzle → Blue Keycard)" << std::endl;
    std::cout << "Room 3: Medieval Weapons (Riddle + Bolt Cutters)" << std::endl;
    std::cout << "Room 4: Security Control (Lock Puzzle → Yellow Keycard)" << std::endl;
    std::cout << "Room 5: Dark Archives (Need Flashlight!)" << std::endl;
    std::cout << "Room 6: Laboratory (Need Bolt Cutters!)" << std::endl;
    std::cout << "Room 7: Director's Office (Get Evidence File = WIN!)" << std::endl;
    std::cout << "==================================\n" << std::endl;
}

// Setup puzzles in rooms
void Game::setupPuzzles() {
    // Room 2: Pattern Puzzle (reward: Blue Keycard)
    auto patternPuzzle = std::make_shared<PatternPuzzle>(std::vector<int>{1, 3, 2, 4});
    patternPuzzle->setFont(mainFont);
    rooms[2]->addPuzzle(patternPuzzle);
    
    // Room 3: Riddle Puzzle
    auto riddle = std::make_shared<RiddlePuzzle>(
        "I speak without a mouth and hear without ears.\nI have no body, but come alive with wind.\nWhat am I?",
        "echo"
    );
    riddle->setFont(mainFont);
    rooms[3]->addPuzzle(riddle);
    
    // Room 4: Lock Puzzle (reward: Yellow Keycard)
    auto lockPuzzle = std::make_shared<LockPuzzle>("4738");
    lockPuzzle->setFont(mainFont);
    rooms[4]->addPuzzle(lockPuzzle);
    
    // Room 5: Math Puzzle (reward: Green Keycard)
    auto mathPuzzle = std::make_shared<MathPuzzle>("(60 - 12) = ?", "048");
    mathPuzzle->setFont(mainFont);
    rooms[5]->addPuzzle(mathPuzzle);
    
    // Room 6: Wire Puzzle (reward: Master Keycard)
    auto wirePuzzle = std::make_shared<WirePuzzle>(
        std::vector<std::string>{"Red", "Yellow", "Blue", "Green", "Purple"}
    );
    wirePuzzle->setFont(mainFont);
    wirePuzzle->setBoltCutters(false); // Will be enabled when player has bolt cutters
    rooms[6]->addPuzzle(wirePuzzle);
    
    std::cout << "All Puzzles setup complete." << std::endl;
}

// Main game loop
void Game::run() {
    while (window.isOpen()) {
        deltaTime = clock.restart().asSeconds();
        
        processEvents();
        update();
        render();
    }
}

// Process input events
void Game::processEvents() {
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }
        
        // State-specific input handling
        switch (currentState) {
            case GameState::MENU:
                handleMenuInput(*event);
                break;
            case GameState::PLAYING:
                handlePlayingInput(*event);
                break;
            case GameState::PUZZLE_ACTIVE:
                handlePuzzleInput(*event);
                break;
            case GameState::PAUSED:
                handlePauseInput(*event);
                break;
            default:
                break;
        }
    }
}

// Handle menu input
void Game::handleMenuInput(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Enter) {
            currentState = GameState::PLAYING;
            gameTimer->start();
            showStoryText(1); // Show Room 1 story
            std::cout << "\n=== GAME STARTED ===" << std::endl;
            std::cout << "Year 2089. You've infiltrated the museum." << std::endl;
            std::cout << "Find the Evidence File and escape!" << std::endl;
            std::cout << "Controls: WASD=Move, E=Items/Doors, P=Puzzles, I=Inventory\n" << std::endl;
        }
    }
}

// Handle playing state input
void Game::handlePlayingInput(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        // Pause game
        if (keyPressed->code == sf::Keyboard::Key::Escape) {
            pauseGame();
        }
        // Toggle inventory
        if (keyPressed->code == sf::Keyboard::Key::I) {
            inventory->toggleVisibility();
        }
        // E = Pick up items and interact with doors
        if (keyPressed->code == sf::Keyboard::Key::E) {
            checkDoorInteraction();
            checkItemPickup();
        }
        // P = Activate puzzles
        if (keyPressed->code == sf::Keyboard::Key::P) {
            checkPuzzleInteraction();
        }
    }
}

// Handle puzzle input
void Game::handlePuzzleInput(const sf::Event& event) {
    if (activePuzzle) {
        bool wasSolved = activePuzzle->isSolvedStatus();
        
        activePuzzle->handleInput(const_cast<sf::Event&>(event));
        
        // Check if puzzle was just solved
        if (!wasSolved && activePuzzle->isSolvedStatus()) {
            gameTimer->addTime(activePuzzle->getTimeBonus());
            showNotification("Puzzle Solved! +" + std::to_string(activePuzzle->getTimeBonus()) + "s", sf::Color::Green, 3.0f);
            std::cout << "\n*** PUZZLE SOLVED! ***" << std::endl;
            std::cout << "Time bonus: +" << activePuzzle->getTimeBonus() << " seconds" << std::endl;
            
            // Give rewards based on current room
            if (currentRoomID == 2) {
                // Room 2 Pattern Puzzle → Blue Keycard
                auto blueCard = std::make_shared<Key>("Blue Keycard", "blue_keycard", 650.0f, 500.0f);
                rooms[2]->addItem(blueCard);
                showNotification("Blue Keycard appeared! Check bottom-right corner!", sf::Color::Cyan, 4.0f);
                std::cout << "🔑 BLUE KEYCARD unlocked! (Unlocks Room 3)" << std::endl;
            } else if (currentRoomID == 4) {
                // Room 4 Lock Puzzle → Yellow Keycard
                auto yellowCard = std::make_shared<Key>("Yellow Keycard", "yellow_keycard", 650.0f, 500.0f);
                rooms[4]->addItem(yellowCard);
                showNotification("Yellow Keycard appeared! Check bottom-right corner!", sf::Color::Yellow, 4.0f);
                std::cout << "🔑 YELLOW KEYCARD unlocked! (Unlocks Room 5 - Dark Archives)" << std::endl;
            } else if (currentRoomID == 5) {
                // Room 5 Math Puzzle → Green Keycard
                auto greenCard = std::make_shared<Key>("Green Keycard", "green_keycard", 650.0f, 500.0f);
                rooms[5]->addItem(greenCard);
                showNotification("Green Keycard appeared! Safe unlocked!", sf::Color::Green, 4.0f);
                std::cout << "🔑 GREEN KEYCARD unlocked! (Unlocks Room 6 - Laboratory)" << std::endl;
            } else if (currentRoomID == 6) {
                // Room 6 Wire Puzzle → Master Keycard
                auto masterCard = std::make_shared<Key>("Master Keycard", "master_keycard", 650.0f, 500.0f);
                rooms[6]->addItem(masterCard);
                showNotification("Master Keycard appeared! Alarm disabled!", sf::Color(255, 215, 0), 4.0f);
                std::cout << "🔑 MASTER KEYCARD unlocked! (Unlocks Room 7 - Director's Office!)" << std::endl;
            }
        }
    }
    
    // Exit puzzle with Escape
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Escape) {
            activePuzzle = nullptr;
            currentState = GameState::PLAYING;
            gameTimer->resume();
            std::cout << "Puzzle closed." << std::endl;
        }
    }
}

// Handle pause input
void Game::handlePauseInput(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Escape) {
            resumeGame();
        }
    }
}

// Update game state
void Game::update() {
    switch (currentState) {
        case GameState::MENU:
            updateMenu();
            break;
        case GameState::PLAYING:
            updatePlaying();
            break;
        case GameState::PUZZLE_ACTIVE:
            updatePuzzle();
            break;
        case GameState::GAME_OVER:
            updateGameOver();
            break;
        default:
            break;
    }
}

void Game::updateMenu() {
    // Menu doesn't need updates
}

void Game::updatePlaying() {
    // Update timer
    gameTimer->update(deltaTime);
    
    // Update notification timer
    if (notificationTimer > 0) {
        notificationTimer -= deltaTime;
    }
    
    // Update player
    player->handleInput(deltaTime);
    player->update(deltaTime);
    
    // Update current room
    if (rooms.find(currentRoomID) != rooms.end()) {
        rooms[currentRoomID]->update(deltaTime);
        
        // Update guards in current room
        auto& guards = rooms[currentRoomID]->getGuards();
        for (auto& guard : guards) {
            guard->update(deltaTime, *player);
        }
    }
    
    // Check collisions
    checkCollisions();
    checkGuardDetection();
    
    // Check win/lose conditions
    checkWinCondition();
    checkLoseCondition();
}

void Game::updatePuzzle() {
    if (activePuzzle) {
        activePuzzle->update(deltaTime);
    }
}

void Game::updateGameOver() {
    // Game over doesn't need updates
}

// Render everything
void Game::render() {
    window.clear(sf::Color(20, 20, 30));
    
    switch (currentState) {
        case GameState::MENU:
            renderMenu();
            break;
        case GameState::PLAYING:
            renderPlaying();
            break;
        case GameState::PUZZLE_ACTIVE:
            renderPuzzle();
            break;
        case GameState::GAME_OVER:
            renderGameOver();
            break;
        case GameState::VICTORY:
            renderVictory();
            break;
        default:
            break;
    }
    
    window.display();
}

void Game::renderMenu() {
    stateText.setString("MUSEUM ESCAPE - ENHANCED\n\nYear 2089\nYou're an investigative journalist\nbreaking into the museum to find evidence\nof illegal experiments.\n\nPress ENTER to Start\n\nControls:\nWASD = Move\nE = Items/Doors\nP = Puzzles\nI = Inventory");
    stateText.setCharacterSize(20);
    stateText.setPosition({150.0f, 150.0f});
    window.draw(stateText);
}

void Game::renderPlaying() {
    // Draw current room
    if (rooms.find(currentRoomID) != rooms.end()) {
        rooms[currentRoomID]->draw(window);
    }
    
    // Draw player
    player->draw(window);
    
    // Draw timer
    gameTimer->draw(window);
    
    // Draw room name (top-left)
    sf::Text roomName(mainFont);
    roomName.setString("Room: " + rooms[currentRoomID]->getRoomName());
    roomName.setCharacterSize(18);
    roomName.setFillColor(sf::Color::White);
    roomName.setPosition({10.0f, 10.0f});
    window.draw(roomName);
    
    // Draw inventory if visible
    if (inventory->getVisible()) {
        inventory->draw(window);
    }
    
    // Draw notification if active (ALWAYS ON TOP)
    if (notificationTimer > 0) {
        notificationText.setString(currentNotification);
        notificationText.setFillColor(notificationColor);
        window.draw(notificationText);
    }
}

void Game::renderPuzzle() {
    // Draw dimmed game background
    renderPlaying();
    window.draw(overlay);
    
    // Draw active puzzle
    if (activePuzzle) {
        activePuzzle->display(window);
    }
}

void Game::renderGameOver() {
    window.draw(overlay);
    stateText.setString("GAME OVER\n\nYou ran out of time!\n\nPress ESC to quit");
    stateText.setCharacterSize(30);
    stateText.setPosition({250.0f, 250.0f});
    window.draw(stateText);
}

void Game::renderVictory() {
    window.draw(overlay);
    stateText.setString("MISSION ACCOMPLISHED!\n\nYou escaped with the evidence!\n\nPress ESC to quit");
    stateText.setCharacterSize(26);
    stateText.setPosition({180.0f, 230.0f});
    window.draw(stateText);
}

// Change to a different room
// Change to a different room
void Game::changeRoom(int newRoomID) {
    if (rooms.find(newRoomID) != rooms.end()) {
        
        float spawnX = 100.0f;
        float spawnY = 300.0f;

        // MANUAL COORDINATE CONFIGURATION
        switch (newRoomID) {
            case 1: // Entering Main Entrance
                // You can only come back to Room 1 from Room 2
                spawnX = 700.0f; 
                spawnY = 300.0f;
                break;

            case 2: // Entering Ancient Artifacts
                if (currentRoomID == 1) {
                    // Coming from Room 1 (Left Door)
                    spawnX = 100.0f; spawnY = 300.0f;
                } else {
                    // Coming from Room 3 (Right Door)
                    spawnX = 700.0f; spawnY = 300.0f;
                }
                break;

            case 3: // Entering Medieval Weapons
                if (currentRoomID == 2) {
                    spawnX = 100.0f; spawnY = 300.0f;
                } else {
                    spawnX = 700.0f; spawnY = 300.0f;
                }
                break;

            case 4: // Entering Security Control
                if (currentRoomID == 3) {
                    spawnX = 100.0f; spawnY = 300.0f;
                } else {
                    spawnX = 700.0f; spawnY = 300.0f;
                }
                break;

            case 5: // Entering Dark Archives
                if (currentRoomID == 4) {
                    spawnX = 10.0f; spawnY = 30.0f;
                } else {
                    spawnX = 700.0f; spawnY = 300.0f;
                }
                break;

            case 6: // Entering Laboratory
                if (currentRoomID == 5) {
                    spawnX = 100.0f; spawnY = 300.0f;
                } else {
                    spawnX = 700.0f; spawnY = 300.0f;
                }
                break;

            case 7: // Entering Director's Office
                // Can only enter from Room 6
                spawnX = 100.0f; 
                spawnY = 300.0f;
                break;
        }

        currentRoomID = newRoomID;
        rooms[currentRoomID]->setVisited(true);
        
        // Apply the manually set position
        player->setPosition(spawnX, spawnY);
        
        // Show story text for new room
        showStoryText(newRoomID);
        
        // Trigger Solved Background if entering a previously solved room
        // rooms[currentRoomID]->revealSolvedBackground();
        
        std::cout << "\n→ Entered: " << rooms[currentRoomID]->getRoomName() << std::endl;
    }
}

// Show story text when entering room
void Game::showStoryText(int roomID) {
    std::map<int, std::string> roomStories = {
        {1, "You've infiltrated the museum. Security drones patrol ahead."},
        {2, "Ancient artifacts glow eerily. A color puzzle guards the path."},
        {3, "Medieval weapons surround you. An ancient riddle blocks your way."},
        {4, "Security monitors flicker. Guards patrol heavily here."},
        {5, "Darkness engulfs everything. Good thing you have that flashlight..."},
        {6, "Bubbling beakers and strange equipment. The lab where it happened."},
        {7, "The Director's Office. The evidence file is here!"}
    };
    
    if (roomStories.find(roomID) != roomStories.end()) {
        showNotification(roomStories[roomID], sf::Color(200, 200, 255), 5.0f);
    }
}

// Activate a puzzle
void Game::activatePuzzle(std::shared_ptr<Puzzle> puzzle) {
    activePuzzle = puzzle;
    currentState = GameState::PUZZLE_ACTIVE;
    gameTimer->pause();
    std::cout << "Puzzle activated! (Press ESC to exit)" << std::endl;
}

// Check collisions with room boundaries
void Game::checkCollisions() {
    auto playerBounds = player->getBounds();
    sf::Vector2f pos = player->getPosition();
    
    if (pos.x < 0) player->setPosition(0, pos.y);
    if (pos.y < 0) player->setPosition(pos.x, 0);
    if (pos.x > 800 - playerBounds.size.x) player->setPosition(800 - playerBounds.size.x, pos.y);
    if (pos.y > 600 - playerBounds.size.y) player->setPosition(pos.x, 600 - playerBounds.size.y);
}

// Check if guards detect player
void Game::checkGuardDetection() {
    auto& guards = rooms[currentRoomID]->getGuards();
    
    for (auto& guard : guards) {
        if (guard->detectPlayer(*player)) {
            if (!player->isPlayerWarned()) {
                player->warn();
                showNotification("WARNING! Caught by guard!", sf::Color::Yellow, 3.0f);
                std::cout << "⚠️  WARNING! Caught by guard!" << std::endl;
                gameTimer->subtractTime(5.0f);
            } else {
                showNotification("CAUGHT AGAIN! Game Over!", sf::Color::Red, 2.0f);
                std::cout << "💀 CAUGHT AGAIN! GAME OVER!" << std::endl;
                setGameOver(false);
                return;
            }
        }
    }
}

// Check door interactions
void Game::checkDoorInteraction() {
    auto& doors = rooms[currentRoomID]->getDoors();
    auto playerBounds = player->getBounds();
    
    for (auto& door : doors) {
        if (door->checkCollision(playerBounds)) {
            if (door->getLockedStatus()) {
                bool hasKey = false;
                std::string requiredKey = "";
                
                int targetRoom = door->getTargetRoomID();
                if (targetRoom == 3) {
                    requiredKey = "Blue Keycard";
                } else if (targetRoom == 5) {
                    requiredKey = "Yellow Keycard";
                    // Check for flashlight
                    if (!inventory->hasTool("flashlight")) {
                        showNotification("It's too dark ahead! Need a Flashlight!", sf::Color::Red, 3.0f);
                        std::cout << "🔦 Room 5 is DARK! You need a Flashlight first!" << std::endl;
                        return;
                    }
                } else if (targetRoom == 6) {
                    requiredKey = "Green Keycard";
                    // Check for bolt cutters BEFORE checking keycard
                    if (!inventory->hasTool("bolt_cutters")) {
                        showNotification("Door is chained! Need Bolt Cutters!", sf::Color::Red, 3.0f);
                        std::cout << "✂️ Door is chained! You need Bolt Cutters!" << std::endl;
                        return;
                    }
                } else if (targetRoom == 7) {
                    requiredKey = "Master Keycard";
                }
                
                auto& inv = player->getInventory();
                for (auto* item : inv) {
                    if (item->getName() == requiredKey) {
                        hasKey = true;
                        break;
                    }
                }
                
                if (hasKey) {
                    door->unlock();
                    showNotification("Door unlocked with " + requiredKey + "!", sf::Color::Green, 2.0f);
                    std::cout << "🔓 Door unlocked with " << requiredKey << "!" << std::endl;
                    changeRoom(door->getTargetRoomID());
                } else {
                    showNotification("LOCKED! Need " + requiredKey, sf::Color::Red, 2.0f);
                    std::cout << "🔒 Door LOCKED! Need: " << requiredKey << std::endl;
                }
            } else {
                changeRoom(door->getTargetRoomID());
            }
            return;
        }
    }
}

// Check item pickup
void Game::checkItemPickup() {
    auto& items = rooms[currentRoomID]->getItems();
    auto playerBounds = player->getBounds();
    
    for (auto& item : items) {
        if (!item->isItemCollected() && item->checkCollision(playerBounds)) {
            item->collect();
            player->addItem(item.get());
            inventory->addItem(item);
            
            // Special messages for important items
            if (item->getName() == "Flashlight") {
                showNotification("🔦 FLASHLIGHT acquired! You can now enter dark areas!", sf::Color::Yellow, 5.0f);
                std::cout << "🔦 FLASHLIGHT acquired! Room 5 (Dark Archives) is now accessible!" << std::endl;
            } else if (item->getName() == "Bolt Cutters") {
                showNotification("✂️ BOLT CUTTERS acquired! You can now cut chains!", sf::Color::Yellow, 5.0f);
                std::cout << "✂️ BOLT CUTTERS acquired! You can access chained areas!" << std::endl;
            } else if (item->getName() == "Evidence File") {
                showNotification("📄 EVIDENCE FILE acquired! Escape to win!", sf::Color::Green, 5.0f);
                std::cout << "📄 EVIDENCE FILE acquired! Mission complete - now escape!" << std::endl;
                // Trigger victory
                setGameOver(true);
            } else if (item->getName() == "Access Code Note") {
                Passcode* passcode = dynamic_cast<Passcode*>(item.get());
                if (passcode) {
                    std::string code = passcode->getCode();
                    showNotification("Security Code: " + code + " (for Lock Puzzle!)", sf::Color::Cyan, 8.0f);
                    std::cout << "📜 Found Access Code: " << code << std::endl;
                }
            } else {
                showNotification("Picked up: " + item->getName(), sf::Color::Cyan, 2.0f);
                std::cout << "📦 Picked up: " << item->getName() << std::endl;
            }
        }
    }
}

// Check puzzle interaction
void Game::checkPuzzleInteraction() {
    auto& puzzles = rooms[currentRoomID]->getPuzzles();
    
    for (auto& puzzle : puzzles) {
        if (!puzzle->isSolvedStatus()) {
            // Special handling for Wire Puzzle - need bolt cutters
            if (currentRoomID == 6) {
                WirePuzzle* wirePuzzle = dynamic_cast<WirePuzzle*>(puzzle.get());
                if (wirePuzzle) {
                    if (!inventory->hasTool("bolt_cutters")) {
                        showNotification("Need Bolt Cutters to cut wires!", sf::Color::Red, 3.0f);
                        std::cout << "✂️ Wire Puzzle requires Bolt Cutters!" << std::endl;
                        return;
                    }
                    wirePuzzle->setBoltCutters(true);
                }
            }
            
            activatePuzzle(puzzle);
            showNotification("Puzzle activated! Press ESC to close", sf::Color::Magenta, 2.0f);
            return;
        }
    }
}

// Check if player has won
void Game::checkWinCondition() {
    // Win condition: Have Evidence File
    if (inventory->hasItem("Evidence File")) {
        setGameOver(true);
    }
}

// Check if player has lost
void Game::checkLoseCondition() {
    if (gameTimer->isExpired()) {
        setGameOver(false);
    }
}

// Set game over state
void Game::setGameOver(bool victory) {
    if (victory) {
        currentState = GameState::VICTORY;
        std::cout << "\n🎉 MISSION ACCOMPLISHED! 🎉" << std::endl;
        std::cout << "You escaped with the evidence!" << std::endl;
    } else {
        currentState = GameState::GAME_OVER;
        std::cout << "\n💀 GAME OVER! 💀" << std::endl;
    }
    gameTimer->stop();
}

// Pause the game
void Game::pauseGame() {
    currentState = GameState::PAUSED;
    gameTimer->pause();
}

// Resume the game
void Game::resumeGame() {
    currentState = GameState::PLAYING;
    gameTimer->resume();
}

// Reset game to initial state
void Game::resetGame() {
    currentState = GameState::MENU;
    gameTimer->reset();
}

// Show notification on screen
void Game::showNotification(const std::string& message, const sf::Color& color, float duration) {
    currentNotification = message;
    notificationColor = color;
    notificationTimer = duration;
}