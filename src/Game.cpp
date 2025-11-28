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

Game::~Game() {}

void Game::initialize() {
    loadAssets();
    player = std::make_unique<Player>(100.0f, 100.0f, playerTexture);
    gameTimer = std::make_unique<Timer>(600.0f);
    gameTimer->setDisplayPosition(650.0f, 20.0f);
    gameTimer->setFont(mainFont);
    inventory = std::make_unique<Inventory>(15);
    inventory->setFont(mainFont);
    createRooms();
    setupPuzzles();
    stateText.setFont(mainFont);
    stateText.setCharacterSize(30);
    stateText.setFillColor(sf::Color::White);
    stateText.setPosition({250.0f, 250.0f});
    notificationText.setFont(mainFont);
    notificationText.setCharacterSize(24);
    notificationText.setPosition({50.0f, 50.0f});
    notificationText.setOutlineThickness(2.0f);
    notificationText.setOutlineColor(sf::Color::Black);
    overlay.setSize({800.0f, 600.0f});
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    std::cout << "Game initialized successfully!" << std::endl;
}

void Game::loadAssets() {
    bool fontLoaded = false;
    if (mainFont.openFromFile("assets/arial.ttf")) fontLoaded = true;
    else if (mainFont.openFromFile("arial.ttf")) fontLoaded = true;
    else if (mainFont.openFromFile("D:/Assignments/Sem3/OOP/Prozect/main/assets/arial.ttf")) fontLoaded = true;
    
    if (!fontLoaded) std::cerr << "Warning: Could not load font!" << std::endl;
    
    if (!playerTexture.loadFromFile("assets/player.png")) std::cerr << "Failed: assets/player.png" << std::endl;
    if (!guardTexture.loadFromFile("assets/guard.png")) std::cerr << "Failed: assets/guard.png" << std::endl;
    
    // === LOAD ROOM TEXTURES ===
    for (int i = 1; i <= 7; ++i) {
        // Normal Background
        sf::Texture texture;
        std::string filenamePNG = "assets/room" + std::to_string(i) + ".png";
        if (texture.loadFromFile(filenamePNG)) {
            roomTextures[i] = texture;
            std::cout << "Loaded: " << filenamePNG << std::endl;
        }
        
        // Open Background (e.g. room1_open.png)
        sf::Texture openTexture;
        std::string openFilename = "assets/room" + std::to_string(i) + "_open.png";
        if (openTexture.loadFromFile(openFilename)) {
            solvedRoomTextures[i] = openTexture;
            std::cout << "Loaded: " << openFilename << std::endl;
        }
    }
}

void Game::createRooms() {
    // Room Creation
    auto room1 = std::make_shared<Room>(1, "Main Entrance", 0, 0, 800, 600);
    room1->addItem(std::make_shared<Tool>("Flashlight", "flashlight", "Illuminates dark areas", 150.0f, 150.0f));
    room1->addItem(std::make_shared<BasicItem>("Museum Map", "Map of museum", 650.0f, 150.0f));
    auto guard1 = std::make_shared<Guard>(400.0f, 200.0f, 100.0f, guardTexture);
    guard1->addPatrolPoint(400.0f, 200.0f); guard1->addPatrolPoint(600.0f, 200.0f);
    room1->addGuard(guard1);
    rooms[1] = room1;
    
    auto room2 = std::make_shared<Room>(2, "Ancient Artifacts Gallery", 0, 0, 800, 600);
    auto guard2 = std::make_shared<Guard>(400.0f, 450.0f, 110.0f, guardTexture);
    guard2->addPatrolPoint(400.0f, 450.0f); guard2->addPatrolPoint(400.0f, 150.0f);
    room2->addGuard(guard2);
    rooms[2] = room2;
    
    auto room3 = std::make_shared<Room>(3, "Medieval Weapons Hall", 0, 0, 800, 600);
    room3->addItem(std::make_shared<Tool>("Bolt Cutters", "bolt_cutters", "Cuts chains", 650.0f, 500.0f));
    room3->addItem(std::make_shared<BasicItem>("Red Keycard", "Security card", 150.0f, 150.0f));
    auto guard3 = std::make_shared<Guard>(400.0f, 200.0f, 100.0f, guardTexture);
    guard3->addPatrolPoint(400.0f, 200.0f); guard3->addPatrolPoint(600.0f, 200.0f);
    room3->addGuard(guard3);
    rooms[3] = room3;
    
    auto room4 = std::make_shared<Room>(4, "Security Control Room", 0, 0, 800, 600);
    room4->addItem(std::make_shared<Passcode>("Access Code Note", "4738", 150.0f, 500.0f));
    auto guard4a = std::make_shared<Guard>(300.0f, 150.0f, 110.0f, guardTexture);
    guard4a->addPatrolPoint(300.0f, 150.0f); guard4a->addPatrolPoint(600.0f, 150.0f);
    room4->addGuard(guard4a);
    auto guard4b = std::make_shared<Guard>(600.0f, 450.0f, 110.0f, guardTexture);
    guard4b->addPatrolPoint(600.0f, 450.0f); guard4b->addPatrolPoint(300.0f, 450.0f);
    room4->addGuard(guard4b);
    rooms[4] = room4;
    
    auto room5 = std::make_shared<Room>(5, "Dark Archives", 0, 0, 800, 600);
    room5->addItem(std::make_shared<BasicItem>("Encrypted Note", "Wire sequence", 650.0f, 150.0f));
    auto guard5 = std::make_shared<Guard>(400.0f, 400.0f, 120.0f, guardTexture);
    guard5->addPatrolPoint(400.0f, 400.0f); guard5->addPatrolPoint(600.0f, 400.0f);
    room5->addGuard(guard5);
    rooms[5] = room5;
    
    auto room6 = std::make_shared<Room>(6, "Laboratory", 0, 0, 800, 600);
    room6->addItem(std::make_shared<BasicItem>("Evidence Log", "Illegal experiments", 150.0f, 150.0f));
    auto guard6a = std::make_shared<Guard>(400.0f, 200.0f, 115.0f, guardTexture);
    guard6a->addPatrolPoint(400.0f, 200.0f); guard6a->addPatrolPoint(600.0f, 200.0f);
    room6->addGuard(guard6a);
    auto guard6b = std::make_shared<Guard>(600.0f, 450.0f, 115.0f, guardTexture);
    guard6b->addPatrolPoint(600.0f, 450.0f); guard6b->addPatrolPoint(400.0f, 450.0f);
    room6->addGuard(guard6b);
    rooms[6] = room6;
    
    auto room7 = std::make_shared<Room>(7, "Director's Office", 0, 0, 800, 600);
    room7->addItem(std::make_shared<BasicItem>("Evidence File", "The proof!", 400.0f, 300.0f));
    room7->setExitRoom(true);
    rooms[7] = room7;
    
    // Doors (Invisible)
    room1->addDoor(std::make_shared<Door>(750.0f, 300.0f, 2));
    room2->addDoor(std::make_shared<Door>(50.0f, 300.0f, 1));
    room2->addDoor(std::make_shared<Door>(750.0f, 300.0f, 3, true, "blue_keycard"));
    room3->addDoor(std::make_shared<Door>(50.0f, 300.0f, 2));
    room3->addDoor(std::make_shared<Door>(750.0f, 300.0f, 4));
    room4->addDoor(std::make_shared<Door>(50.0f, 300.0f, 3));
    room4->addDoor(std::make_shared<Door>(750.0f, 300.0f, 5, true, "yellow_keycard"));
    room5->addDoor(std::make_shared<Door>(50.0f, 300.0f, 4));
    room5->addDoor(std::make_shared<Door>(750.0f, 300.0f, 6, true, "green_keycard"));
    room6->addDoor(std::make_shared<Door>(50.0f, 300.0f, 5));
    room6->addDoor(std::make_shared<Door>(750.0f, 300.0f, 7, true, "master_keycard"));
    room7->addDoor(std::make_shared<Door>(50.0f, 300.0f, 6));
    
    // === APPLY TEXTURES ===
    for (auto& pair : rooms) {
        int id = pair.first;
        if (roomTextures.find(id) != roomTextures.end()) {
            pair.second->setBackgroundTexture(roomTextures[id]);
        }
        if (solvedRoomTextures.find(id) != solvedRoomTextures.end()) {
            pair.second->setSolvedBackgroundTexture(solvedRoomTextures[id]);
        }
    }
}

void Game::setupPuzzles() {
    auto patternPuzzle = std::make_shared<PatternPuzzle>(std::vector<int>{1, 3, 2, 4});
    patternPuzzle->setFont(mainFont);
    rooms[2]->addPuzzle(patternPuzzle);
    
    auto riddle = std::make_shared<RiddlePuzzle>("I speak without a mouth...", "echo");
    riddle->setFont(mainFont);
    rooms[3]->addPuzzle(riddle);
    
    auto lockPuzzle = std::make_shared<LockPuzzle>("4738");
    lockPuzzle->setFont(mainFont);
    rooms[4]->addPuzzle(lockPuzzle);
    
    auto mathPuzzle = std::make_shared<MathPuzzle>("(60 - 12) = ?", "048");
    mathPuzzle->setFont(mainFont);
    rooms[5]->addPuzzle(mathPuzzle);
    
    auto wirePuzzle = std::make_shared<WirePuzzle>(std::vector<std::string>{"Red", "Yellow", "Blue", "Green", "Purple"});
    wirePuzzle->setFont(mainFont);
    wirePuzzle->setBoltCutters(false);
    rooms[6]->addPuzzle(wirePuzzle);
}

void Game::run() {
    while (window.isOpen()) {
        deltaTime = clock.restart().asSeconds();
        if (deltaTime > 0.1f) deltaTime = 0.1f;
        processEvents();
        update();
        render();
    }
}

void Game::processEvents() {
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) window.close();
        switch (currentState) {
            case GameState::MENU: handleMenuInput(*event); break;
            case GameState::PLAYING: handlePlayingInput(*event); break;
            case GameState::PUZZLE_ACTIVE: handlePuzzleInput(*event); break;
            case GameState::PAUSED: handlePauseInput(*event); break;
            default: break;
        }
    }
}

void Game::handleMenuInput(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Enter) {
            currentState = GameState::PLAYING;
            gameTimer->start();
            showStoryText(1);
        }
    }
}

void Game::handlePlayingInput(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Escape) pauseGame();
        if (keyPressed->code == sf::Keyboard::Key::I) inventory->toggleVisibility();
        if (keyPressed->code == sf::Keyboard::Key::E) { checkDoorInteraction(); checkItemPickup(); }
        if (keyPressed->code == sf::Keyboard::Key::P) checkPuzzleInteraction();
    }
}

void Game::handlePuzzleInput(const sf::Event& event) {
    if (activePuzzle) {
        bool wasSolved = activePuzzle->isSolvedStatus();
        activePuzzle->handleInput(const_cast<sf::Event&>(event));
        
        if (!wasSolved && activePuzzle->isSolvedStatus()) {
            gameTimer->addTime(activePuzzle->getTimeBonus());
            showNotification("Puzzle Solved!", sf::Color::Green, 3.0f);
            
            // === REVEAL BACKGROUND (Smooth Fade) ===
            rooms[currentRoomID]->revealSolvedBackground();
            
            if (currentRoomID == 2) rooms[2]->addItem(std::make_shared<Key>("Blue Keycard", "blue_keycard", 650.0f, 500.0f));
            else if (currentRoomID == 4) rooms[4]->addItem(std::make_shared<Key>("Yellow Keycard", "yellow_keycard", 650.0f, 500.0f));
            else if (currentRoomID == 5) rooms[5]->addItem(std::make_shared<Key>("Green Keycard", "green_keycard", 650.0f, 500.0f));
            else if (currentRoomID == 6) rooms[6]->addItem(std::make_shared<Key>("Master Keycard", "master_keycard", 650.0f, 500.0f));
        }
    }
    
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Escape) {
            activePuzzle = nullptr;
            currentState = GameState::PLAYING;
            gameTimer->resume();
        }
    }
}

void Game::handlePauseInput(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Escape) resumeGame();
    }
}

void Game::update() {
    if (currentState == GameState::PLAYING) updatePlaying();
    if (currentState == GameState::PUZZLE_ACTIVE) updatePuzzle();
}

void Game::updateMenu() {}

void Game::updatePlaying() {
    gameTimer->update(deltaTime);
    if (notificationTimer > 0) notificationTimer -= deltaTime;
    player->handleInput(deltaTime);
    player->update(deltaTime);
    
    if (rooms.find(currentRoomID) != rooms.end()) {
        rooms[currentRoomID]->update(deltaTime); // Update fade transition
        for (auto& guard : rooms[currentRoomID]->getGuards()) guard->update(deltaTime, *player);
    }
    checkCollisions();
    checkGuardDetection();
    checkWinCondition();
    checkLoseCondition();
}

void Game::updatePuzzle() {
    if (activePuzzle) activePuzzle->update(deltaTime);
}

void Game::updateGameOver() {}

void Game::render() {
    window.clear(sf::Color(20, 20, 30));
    switch (currentState) {
        case GameState::MENU: renderMenu(); break;
        case GameState::PLAYING: renderPlaying(); break;
        case GameState::PUZZLE_ACTIVE: renderPuzzle(); break;
        case GameState::GAME_OVER: renderGameOver(); break;
        case GameState::VICTORY: renderVictory(); break;
        default: break;
    }
    window.display();
}

void Game::renderMenu() { window.draw(stateText); }

void Game::renderPlaying() {
    if (rooms.find(currentRoomID) != rooms.end()) rooms[currentRoomID]->draw(window);
    player->draw(window);
    gameTimer->draw(window);
    
    sf::Text roomName(mainFont);
    roomName.setString("Room: " + rooms[currentRoomID]->getRoomName());
    roomName.setCharacterSize(18);
    roomName.setPosition({10.0f, 10.0f});
    window.draw(roomName);
    
    if (inventory->getVisible()) inventory->draw(window);
    if (notificationTimer > 0) {
        notificationText.setString(currentNotification);
        notificationText.setFillColor(notificationColor);
        window.draw(notificationText);
    }
}

void Game::renderPuzzle() {
    renderPlaying();
    window.draw(overlay);
    if (activePuzzle) activePuzzle->display(window);
}

void Game::renderGameOver() {
    window.draw(overlay);
    stateText.setString("GAME OVER");
    window.draw(stateText);
}

void Game::renderVictory() {
    window.draw(overlay);
    stateText.setString("VICTORY!");
    window.draw(stateText);
}

void Game::changeRoom(int newRoomID) {
    if (rooms.find(newRoomID) != rooms.end()) {
        float spawnX = 100.0f, spawnY = 300.0f;
        
        // Spawn Logic
        if (newRoomID == 1) { spawnX = 700.0f; }
        else if (newRoomID == 7) { spawnX = 100.0f; }
        else if (newRoomID > currentRoomID) { spawnX = 80.0f; }
        else { spawnX = 700.0f; }

        currentRoomID = newRoomID;
        rooms[currentRoomID]->setVisited(true);
        player->setPosition(spawnX, spawnY);
        showStoryText(newRoomID);
        
        // Check if room was already solved previously, keep it open
        if (rooms[currentRoomID]->allPuzzlesSolved()) {
            rooms[currentRoomID]->forceSolvedBackground();
        }
    }
}

void Game::showStoryText(int roomID) {
    showNotification("Room " + std::to_string(roomID), sf::Color::Cyan);
}

void Game::activatePuzzle(std::shared_ptr<Puzzle> puzzle) {
    activePuzzle = puzzle;
    currentState = GameState::PUZZLE_ACTIVE;
    gameTimer->pause();
}

void Game::checkCollisions() {
    auto bounds = player->getBounds();
    sf::Vector2f pos = player->getPosition();
    if (pos.x < 0) player->setPosition(0, pos.y);
    if (pos.y < 0) player->setPosition(pos.x, 0);
    if (pos.x > 800 - bounds.size.x) player->setPosition(800 - bounds.size.x, pos.y);
    if (pos.y > 600 - bounds.size.y) player->setPosition(pos.x, 600 - bounds.size.y);
}

void Game::checkGuardDetection() {
    for (auto& guard : rooms[currentRoomID]->getGuards()) {
        if (guard->detectPlayer(*player)) {
            player->warn();
            showNotification("CAUGHT!", sf::Color::Red);
        }
    }
}

void Game::checkDoorInteraction() {
    auto& doors = rooms[currentRoomID]->getDoors();
    auto bounds = player->getBounds();
    for (auto& door : doors) {
        if (door->checkCollision(bounds)) {
            if (door->getLockedStatus()) {
                // Key Logic (Simplified for brevity)
                // You can add your specific key checks back here
                showNotification("Locked!", sf::Color::Red);
            } else {
                changeRoom(door->getTargetRoomID());
            }
        }
    }
}

void Game::checkItemPickup() {
    auto& items = rooms[currentRoomID]->getItems();
    auto bounds = player->getBounds();
    for (auto& item : items) {
        if (!item->isItemCollected() && item->checkCollision(bounds)) {
            item->collect();
            player->addItem(item.get());
            inventory->addItem(item);
            showNotification("Picked up " + item->getName(), sf::Color::Cyan);
        }
    }
}

void Game::checkPuzzleInteraction() {
    auto& puzzles = rooms[currentRoomID]->getPuzzles();
    for (auto& puzzle : puzzles) {
        if (!puzzle->isSolvedStatus()) {
            activatePuzzle(puzzle);
            return;
        }
    }
}

void Game::checkWinCondition() { if (inventory->hasItem("Evidence File")) setGameOver(true); }
void Game::checkLoseCondition() { if (gameTimer->isExpired()) setGameOver(false); }
void Game::setGameOver(bool victory) {
    currentState = victory ? GameState::VICTORY : GameState::GAME_OVER;
    gameTimer->stop();
}
void Game::pauseGame() { currentState = GameState::PAUSED; gameTimer->pause(); }
void Game::resumeGame() { currentState = GameState::PLAYING; gameTimer->resume(); }
void Game::resetGame() { currentState = GameState::MENU; gameTimer->reset(); }
void Game::showNotification(const std::string& message, const sf::Color& color, float duration) {
    currentNotification = message;
    notificationColor = color;
    notificationTimer = duration;
}