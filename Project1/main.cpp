#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include <SFML/Audio.hpp>

using namespace std;
using namespace sf;

const int WIDTH = 800;
const int HEIGHT = 600;
Event event;
sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "SFML Window");
bool enDeplacement = false;

class Piece {
private:
    Font font;
public:
    Piece() {
        if (!font.loadFromFile("04b_25__.ttf")) {
            cerr << "Erreur : Impossible de charger la police 'ASMAN.TTF'" << endl;
        }
    }

    void creerText(sf::Text& text, const std::string& texte, int charSize, const sf::Color& color, float x, float y) {
        text.setFont(font);
        text.setString(texte);
        text.setCharacterSize(charSize);
        text.setFillColor(color);
        text.setPosition(x, y);
    }

    void creerPieceRectangle(RectangleShape& rectanglePiece, int longueur, int largeur) {
        rectanglePiece.setSize(Vector2f(longueur, largeur));
        rectanglePiece.setFillColor(Color::White);
        rectanglePiece.setOutlineColor(Color::Black);
        rectanglePiece.setOutlineThickness(3);
        rectanglePiece.setPosition(WIDTH / 2 - (longueur / 2), HEIGHT / 2 - (largeur / 2));
    }
};


class Meuble {
private:
    string type;
    string nom;
    RectangleShape meubleShape;

public:
    float x, y;
    float largeur, hauteur; 
    sf::Color couleur;
    RectangleShape& getShape() { return meubleShape; }
    string getNom() const { return nom; } 

    void deplacerMeuble() { 
        if (event.type == Event::KeyPressed) {
            Vector2f pos = meubleShape.getPosition();
            if (event.key.code == Keyboard::Up) pos.y -= 10;
            if (event.key.code == Keyboard::Down) pos.y += 10;
            if (event.key.code == Keyboard::Left) pos.x -= 10;
            if (event.key.code == Keyboard::Right) pos.x += 10;
            meubleShape.setPosition(pos);
        }
    }
    RectangleShape getRectangleShape() const {
        sf::RectangleShape rect(sf::Vector2f(largeur, hauteur));
        rect.setPosition(x, y);
        rect.setFillColor(couleur);
        return rect;
    }
};

class Jeu {
private:
    Piece piece;
    RectangleShape rectanglePiece, rectangleMenu, rectangleMenuAjouter;
    vector<Text> noms;
    vector<RectangleShape> rectShapes;
    Text textMenu, prop1, prop2, menuTitle, option1, option2, option3, option4, option5, creerTextAjouter, afficherMeublesText, nomMeuble;
    RectangleShape checkBox, checkBox2, checkBox1, checkBox2M, checkBox3, checkBox4, checkBox5, newMeuble, rectBleu, styleDebut, styleDebut1, styleDebut2, styleDebut3, styleMenu1, styleMenu2, styleMenu3, styleMenu4, styleMenu5;
    bool firstMenu = true, afficherMenu = false, pieceCree = false, menuAjouterMeuble = false, affichageMeubles = false, autoPlacement = false, supprimerMeubleBool = false;
    bool collisionDetected = false;
    int longueurInt = 0, largeurInt = 0;
    Meuble meuble;
    Texture texture;
    Sprite sprite;
    SoundBuffer buffer;
    Sound sound;

    void creerRectangle(sf::RectangleShape& rectangle, const sf::Vector2f& size, const sf::Color& fillColor,
        const sf::Color& outlineColor, float outlineThickness, const sf::Vector2f& position) {
        rectangle.setSize(size);
        rectangle.setFillColor(fillColor);
        rectangle.setOutlineColor(outlineColor);
        rectangle.setOutlineThickness(outlineThickness);
        rectangle.setPosition(position);
    }

    int captureInput(const string& prompt) {
        string input = "";
        sf::Text textInput;
        piece.creerText(textInput, prompt, 30, Color::Black, 100, 200);

        while (true) {
            window.clear(Color::Black);
            window.draw(sprite);
            window.draw(styleDebut2);
            window.draw(textInput);
            window.display();

            while (window.pollEvent(event)) {
                if (event.type == Event::Closed) window.close();
                if (event.type == Event::TextEntered && event.text.unicode < 128) {
                    if (event.text.unicode == '\b' && !input.empty()) {
                        input.pop_back();
                    }
                    else if (event.text.unicode != '\b') {
                        input += static_cast<char>(event.text.unicode);
                        textInput.setString(prompt + " " + input);
                    }
                }
                if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter) {
                    
                    try {
                        if (stoi(input) > 600)
                        {
                            return 600;
                        }
                        else
                        {
                            return stoi(input);
                        }
                    }
                    catch (...) {
                        input = ""; 
                    }
                }
            }
        }
    }

    string demanderNom() {
        string nom = "";
        Text prompt, input;
        
        piece.creerText(prompt, "Entrez le nom du meuble (15 caracteres max) : ", 30, Color::Black, 60, 150);
        piece.creerText(input, "", 30, Color::Black, 100, 190);

        while (true) {
            window.clear();
            creerRectangle(rectBleu, { 700, 500 }, Color::Blue, Color::Black, 3, { WIDTH / 2 - 350, HEIGHT / 2 - 250 });
            window.draw(rectBleu);
            window.draw(styleMenu1);
            window.draw(prompt);
            input.setString(nom);
            window.draw(input);
            window.display();

            while (window.pollEvent(event)) {
                if (event.type == Event::Closed) window.close(); 
                if (event.type == Event::TextEntered && event.text.unicode < 128) {
                    char enteredChar = static_cast<char>(event.text.unicode);
                    if (enteredChar == '\b' && !nom.empty()) { 
                        nom.pop_back();
                    }
                    else if (enteredChar != '\b') {
                        nom += enteredChar;
                    }
                }
                if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter) {
                    if (nom.size() < 15)
                    {
                        return nom;
                    }
                    
                }
            }
        }
    }

    int demanderDimensions(const string& promptMessage) {
        string input = "";
        Text prompt, inputText;
        piece.creerText(prompt, promptMessage, 30, Color::Black, 60, 150);
        piece.creerText(inputText, "", 30, Color::Black, 100, 190);

        while (true) {
            window.clear();
            rectangleMenuAjouter.setSize(Vector2f(700, 500));
            rectangleMenuAjouter.setFillColor(Color::Blue);
            rectangleMenuAjouter.setOutlineColor(Color::Black);
            rectangleMenuAjouter.setOutlineThickness(3);
            rectangleMenuAjouter.setPosition(WIDTH / 2 - (700 / 2), HEIGHT / 2 - (500 / 2));
            window.draw(rectangleMenuAjouter);
            window.draw(styleMenu1);
            window.draw(prompt);
            inputText.setString(input);
            window.draw(inputText);
            window.display();

            while (window.pollEvent(event)) {
                if (event.type == Event::Closed) {
                    window.close();
                }
                if (event.type == Event::TextEntered) {
                    char enteredChar = static_cast<char>(event.text.unicode);
                    if (enteredChar == '\b' && !input.empty()) {
                        input.pop_back();
                        break;
                    }
                   
                    else if (enteredChar != '\b') {
                        if (isdigit(enteredChar)) {
                            input += enteredChar;
                        }
                    }
                    
                }
                if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter) {
                    try {
                        return stoi(input);
                    }
                    catch (...) {
                        input = "";
                    }
                }
            }
        }
    }

    Text prompt;
    void ajouterMeuble() {
        //ajouter un meuble
        afficherMenu = false;
        menuAjouterMeuble = true;
        string nomDuMeuble = demanderNom();
        int longueurMeuble = demanderDimensions("Longeur :");
        int largeurMeuble = demanderDimensions("largeur :");
        newMeuble.setSize(Vector2f(longueurMeuble, largeurMeuble));
        newMeuble.setFillColor(Color::Green);
        newMeuble.setOutlineColor(Color::Black);
        newMeuble.setOutlineThickness(2);
        newMeuble.setPosition(WIDTH / 2, HEIGHT / 2 );
        piece.creerText(nomMeuble, nomDuMeuble, 10, Color::Blue, WIDTH / 2, HEIGHT / 2 + largeurMeuble);
        enDeplacement = true;
    }

    void afficherMeubles() {
        creerRectangle(rectBleu, { 700, 500 }, Color::Blue, Color::Black, 3, { WIDTH / 2 - 350, HEIGHT / 2 - 250 });
        affichageMeubles = true;
    }

    void supprimerMeuble() {
        supprimerMeubleBool = true;
    }

    void afficherMenuMeubles() {
        afficherMenu = true;
        piece.creerText(menuTitle, "Que voulez-vous faire ?", 40, Color::Black, WIDTH / 2 - 200, 100);

        piece.creerText(option1, "Ajouter un meuble", 30, Color::Black, 150, 190);
        piece.creerText(option2, "Supprimer un meuble", 30, Color::Black, 150, 240);
        piece.creerText(option3, "Afficher les meubles", 30, Color::Black, 150, 290);
        piece.creerText(option4, "Retour", 30, Color::Black, 150, 340);
        piece.creerText(option5, "Quitter", 30, Color::Black, 150, 390);

        creerRectangle(rectangleMenu, { 700, 500 }, Color::Blue, Color::Black, 3, { WIDTH / 2 - 350, HEIGHT / 2 - 250 });
        creerRectangle(checkBox1, { 30, 30 }, Color::White, Color::Black, 2, { 100, 195 });
        creerRectangle(checkBox2M, { 30, 30 }, Color::White, Color::Black, 2, { 100, 245 });
        creerRectangle(checkBox3, { 30, 30 }, Color::White, Color::Black, 2, { 100, 295 });
        creerRectangle(checkBox4, { 30, 30 }, Color::White, Color::Black, 2, { 100, 345 });
        creerRectangle(checkBox5, { 30, 30 }, Color::White, Color::Black, 2, { 100, 395 });
        
        creerRectangle(styleMenu1, { 650, 40 }, Color::Cyan, Color::Black, 3, { WIDTH / 2 - 325, 190 });
        creerRectangle(styleMenu2, { 650, 40 }, Color::Cyan, Color::Black, 3, { WIDTH / 2 - 325, 240 });
        creerRectangle(styleMenu3, { 650, 40 }, Color::Cyan, Color::Black, 3, { WIDTH / 2 - 325, 290 });
        creerRectangle(styleMenu4, { 650, 40 }, Color::Cyan, Color::Black, 3, { WIDTH / 2 - 325, 340 });
        creerRectangle(styleMenu5, { 650, 40 }, Color::Cyan, Color::Black, 3, { WIDTH / 2 - 325, 390 });
    }

    int gestionMenuMeubles() {
        if (!buffer.loadFromFile("sound.wav"))
            return -1;
        Sound sound;
        sound.setBuffer(buffer);
        sound.setVolume(50);
        if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
            Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

            if (checkBox1.getGlobalBounds().contains(mousePos)) {
                sound.play();
                afficherMenu = false;
                ajouterMeuble();
            }
            if (checkBox2M.getGlobalBounds().contains(mousePos)) {
                sound.play();
                afficherMenu = false;
                supprimerMeuble();
            }
            if (checkBox3.getGlobalBounds().contains(mousePos)) {
                sound.play();
                afficherMenu = false;
               afficherMeubles();
            }
            if (checkBox4.getGlobalBounds().contains(mousePos)) {
                sound.play();
                afficherMenu = false;
            }
            if (checkBox5.getGlobalBounds().contains(mousePos)) {
                sound.play();
                window.close();
            }
        }
    }

public:
    int menu() {
        if (!texture.loadFromFile("image.png")) {
            return -1;
        }
        sprite.setTexture(texture);
        sf::Vector2f targetSize(700, 500);

        sprite.setScale(
            targetSize.x / sprite.getLocalBounds().width,
            targetSize.y / sprite.getLocalBounds().height);
        
        piece.creerText(textMenu, "MENU", 50, sf::Color::Black, WIDTH / 2 - 70, 60);
        piece.creerText(prop1, "Creer une nouvelle piece", 30, sf::Color::Black, 200, 200);
        piece.creerText(prop2, "Quitter", 30, sf::Color::Black, 200, 300);

        creerRectangle(styleDebut, { 700, 500 }, Color::Blue, Color::Black, 3, { WIDTH / 2 - 350, HEIGHT / 2 - 250 });
        sprite.setPosition(Vector2f(WIDTH / 2 - 350, HEIGHT / 2 - 250));
        
        
        creerRectangle(styleDebut1, { 650, 60 }, Color::Cyan, Color::Black, 3, { WIDTH / 2 - 325, 280 });
        creerRectangle(styleDebut2, { 650, 60 }, Color::Cyan, Color::Black, 3, { WIDTH / 2 - 325, 180 });

        creerRectangle(checkBox, { 30, 30 }, Color::White, Color::Black, 2, { 100, 200 });
        creerRectangle(checkBox2, { 30, 30 }, Color::White, Color::Black, 2, { 100, 300 });
        return 1;
    }

    int boucleDeJeu() {
        std::vector<sf::RectangleShape> rectShapes;
        menu();  
        
        if (!buffer.loadFromFile("sound.wav"))
            return -1;
        
        sound.setBuffer(buffer);
        Music music;
        if (!music.openFromFile("music.ogg"))
            return -1;
        music.setLoop(true);
        
        while (window.isOpen()) {
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed) {
                    window.close();
                }
                if (event.KeyPressed)
                {
                    if (enDeplacement)
                    {
                        Vector2f pos = newMeuble.getPosition();
                        Vector2f size = newMeuble.getSize();
                        bool collisionDetected = false;
                        Vector2f piecePos = rectanglePiece.getPosition();
                        Vector2f pieceSize = rectanglePiece.getSize();
                        if (event.key.code == Keyboard::Up) pos.y -= 5;
                        if (event.key.code == Keyboard::Down) pos.y += 5;
                        if (event.key.code == Keyboard::Left) pos.x -= 5;
                        if (event.key.code == Keyboard::Right) pos.x += 5;
                        for (auto& autremeuble : rectShapes) {
                            Vector2f otherPos = autremeuble.getPosition();
                            Vector2f otherSize = autremeuble.getSize();
                            if (pos.x < otherPos.x + otherSize.x &&
                                pos.x + size.x > otherPos.x &&
                                pos.y < otherPos.y + otherSize.y &&
                                pos.y + size.y > otherPos.y) {
                                collisionDetected = true;
                                break;
                            }
                        }
                        if (pos.x < piecePos.x || pos.y < piecePos.y ||
                            pos.x + size.x > piecePos.x + pieceSize.x ||
                            pos.y + size.y > piecePos.y + pieceSize.y) {
                            newMeuble.setFillColor(Color::Red);
                            
                        }
                        else {
                            if (collisionDetected) {
                                newMeuble.setFillColor(Color::Red);
                            }
                            else
                            {
                                newMeuble.setFillColor(Color::Green);
                            }
                            if (event.key.code == Keyboard::L) {
                                if (!collisionDetected)
                                {
                                    sound.play();
                                    rectShapes.push_back(newMeuble);
                                    noms.push_back(nomMeuble);
                                    enDeplacement = false;
                                }
                            }
                        }

                        if (pos.x < 0) pos.x = 0;
                        if (pos.y < 0) pos.y = 0;
                        if (pos.x + size.x > WIDTH) pos.x = WIDTH - size.x;
                        if (pos.y + size.y > HEIGHT) pos.y = HEIGHT - size.y;

                        newMeuble.setPosition(pos);
                        nomMeuble.setPosition(Vector2f(pos.x, pos.y + size.y));
                    }
                }
            
                if (firstMenu) {
                    if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                        if (checkBox.getGlobalBounds().contains(window.mapPixelToCoords(Mouse::getPosition(window)))) {
                            longueurInt = captureInput("Entrez la longueur (0-600):");
                            sound.play();
                            largeurInt = captureInput("Entrez la largeur (0-600):");
                            sound.play();

                            piece.creerPieceRectangle(rectanglePiece, longueurInt, largeurInt);
                            pieceCree = true;
                            firstMenu = false;
                        }
                        if (checkBox2.getGlobalBounds().contains(window.mapPixelToCoords(Mouse::getPosition(window)))) {
                            sound.play();
                            window.close();
                        }
                    }
                }
                else if (pieceCree) {
                    if (event.type == Event::KeyPressed && event.key.code == Keyboard::M) {
                        sound.play();
                        afficherMenuMeubles();
                    }
                    if (afficherMenu) {
                        sound.play();
                        gestionMenuMeubles();
                    }
                }
                if (menuAjouterMeuble)
                {
                    window.draw(newMeuble);
                    window.draw(nomMeuble);
                }
                while (affichageMeubles)
                {
                    sound.play();
                    window.clear();
                    string meublesNom = "Les meubles sont : ";
                    for (auto& nom : noms)
                    {
                        meublesNom += nom.getString();
                        meublesNom += ", ";
                    }
                    meublesNom += " \n\nappuyez sur R pour quitter";
                    piece.creerText(afficherMeublesText, meublesNom, 30, Color::Black, 150, 190);
                    window.draw(sprite);
                    window.draw(afficherMeublesText);
                    window.display();
                    if (Keyboard::isKeyPressed(Keyboard::R))
                    {
                        affichageMeubles = false;
                    }
                } 
                while (supprimerMeubleBool)
                {
                    sound.play();
                    window.clear();
                    string meublesNom = "Les meubles sont : ";
                    int i = 0;
                    for (auto& nom : noms)
                    {
                        i++;
                        meublesNom += i;
                        meublesNom += nom.getString();
                        meublesNom += ", ";
                    }
                    meublesNom += " \n\nappuyez sur S pour passer a la suite";
                    piece.creerText(afficherMeublesText, meublesNom, 30, Color::Black, 150, 190);
                    window.draw(sprite);
                    window.draw(afficherMeublesText);
                    window.display();
                    if (Keyboard::isKeyPressed(Keyboard::S))
                    {
                        int nom = captureInput("Entrez le numero correspondant : ");
                        if (!rectShapes.empty()) {
                            rectShapes.erase(rectShapes.begin() + nom - 1);
                            noms.erase(noms.begin() + nom - 1);
                        }
                        supprimerMeubleBool = false;
                    }  
                }
            }
            window.clear();
            if (firstMenu) {
                sound.play();
                window.draw(sprite);
                window.draw(styleDebut1);
                window.draw(styleDebut2);
                window.draw(textMenu);
                window.draw(prop1);
                window.draw(prop2);
                window.draw(checkBox);
                window.draw(checkBox2);
            }
            else {
                if (pieceCree && !affichageMeubles) {
                    window.draw(rectanglePiece);
                    for (const auto& meuble : rectShapes)
                    {
                        window.draw(meuble);
                    } 
                    for ( const auto& nom : noms)
                    {
                        window.draw(nom);
                    }
                    if (menuAjouterMeuble) {
                        window.draw(newMeuble); 
                        window.draw(nomMeuble);
                    }
                }
                if (afficherMenu) {
                    sound.play();
                    window.draw(sprite);
                    window.draw(styleMenu1);
                    window.draw(styleMenu2);
                    window.draw(styleMenu3);
                    window.draw(styleMenu4);
                    window.draw(styleMenu5);
                    window.draw(menuTitle);
                    window.draw(option1);
                    window.draw(option2);
                    window.draw(option3);
                    window.draw(option4);
                    window.draw(option5);
                    window.draw(checkBox1);
                    window.draw(checkBox2M);
                    window.draw(checkBox3);
                    window.draw(checkBox4);
                    window.draw(checkBox5);
                }
            }
            window.display();
        }
    }
};

int main() {
    Music music;
    if (!music.openFromFile("music.ogg"))
        return -1;
    music.setLoop(true);
    music.setPitch(0.5);
    music.play();

    Jeu jeu;
    jeu.boucleDeJeu();
    
    return 0;
}
