#pragma once
#include <Windows.h>

static const char text[] =
"The onlookers go rigid when the train goes past.\n"
"“If he should forever ahsk me.” The ah, released from the\n"
"sentence, flew off like a ball on the meadow.\n"
"His gravity is the death of me. His head in its collar, his\n"
"hair arranged immovably on his skull, the muscles of his\n"
"jowls below, tensed in their places——\n"
"Are the woods still there? The woods were still almost\n"
"there. But hardly had my glance gone ten steps farther\n"
"when I left off, again caught up in the tedious conversation.\n"
"In the dark woods, on the sodden ground, I found my\n"
"way only by the whiteness of his collar.\n"
"In a dream I asked the dancer Eduardova ? to dance the\n"
"Czardas just one time more. She had a broad streak of\n"
"shadow or light across the middle of her face between the\n"
"lower part of her forehead and the cleft of her chin. Just\n"
"then someone with the loathsome gestures of an unconscious intriguer approached "
"to tell her the train was leaving\n"
"immediately. The manner in which she listened to this announcement made it terri"
"bly clear to me that she would\n"
"not dance again. “I am a wicked, evil woman, am I not?”\n"
"she said. “Oh no,” I said, “not that,” and turned away aimlessly.\n"
"Before that I had questioned her about the many flowers \n"
"\n"
"that were stuck into her girdle. “They are from all the\n"
"princes of Europe,” said she. I pondered as to what this\n"
"might mean—that all those fresh flowers stuck in her girdle\n"
"had been presented to the dancer Eduardova by all the\n"
"princes cf Europe.\n"
"The dancer Eduardova, a lover of music, travels in the\n"
"trolley, as everywhere else, in the company of two vigorous\n"
"violinists whom she has play often. For there is no known\n"
"reason why one should not play in the trolley if the playing is good, pleasing t"
"o the fellow passengers, and costs\n"
"nothing; i.e., if the hat is not passed around afterward. Of\n"
"course, at first it is a little surprising and for a short while\n"
"everybody finds it improper. But at full speed, in a strong\n"
"breeze and on a silent street, it sounds quite nice.\n"
"The dancer Eduardova is not as pretty in the open air\n"
"as on the stage. Her faded color, her cheekbones which\n"
"draw her skin so taut that there is scarcely a trace of motion\n"
"in her face and a real face is no longer possible, the large\n"
"nose, which rises as though out of a cavity, with which one\n"
"can take no liberties—such as testing the hardness of the\n"
"point or taking it gently by the bridge and pulling it back\n"
"and forth while one says, “But now you come along.” The\n"
"large figure with the high waist in skirts with too many\n"
"pleats—whom can that please?—she looks like one of my\n"
"aunts, an elderly lady; many elderly aunts of many people\n"
"look like that. In the open air Eduardova really has nothing\n"
"to compensate for these disadvantages, moreover, aside\n"
"from her very good feet; there is actually nothing that\n"
"would give occasion for enthusiasm, astonishment or even\n"
"for respect. And so I have actually seen Eduardova very\n"
"often treated with a degree of indifference that even gen- \n"
"\n"
"tlemen, who were otherwise very adroit, very correct,\n"
"could not conceal, although they naturally made every\n"
"effort to do so in the presence of so famous a dancer as\n"
"Eduardova still was.\n"
"The auricle of my ear felt fresh, rough, cool, succulent\n"
"as a leaf, to the touch.\n"
"I write this very decidedly out of despair over my body\n"
"and over a future with this body.\n"
"When despair shows itself so definitely, is so tied to its\n"
"object, so pent up, as in a soldier who covers a retreat and\n"
"thus lets himself be torn to pieces, then it is not true despair.\n"
"True despair overreaches its goal immediately and always,\n"
"(at this comma it became clear that only the first sentence\n"
"was correct).\n"
"Do you despair?\n"
"Yes? You despair?\n"
"You run away? You want to hide?\n"
"I passed by the brothel as though past the house of a beloved.\n"
"Writers speak a stench.\n"
"The seamstresses in the downpour of rain.?\n"
"Finally, after five months of my life during which I\n"
"could write nothing that would have satisfied me, and for\n"
"which no power will compensate me, though all were under\n"
"obligation to do so, it occurs to me to talk to myself again.\n"
"Whenever I really questioned myself, there was always a\n"
"response forthcoming, there was always something in me\n"
"to catch fire, in this heap of straw that I have been for five "
;

const char* randomsentence() {
    // Count sentences first
    size_t count = 0;
    for (const char* p = text; *p; p++) {
        if (*p == '\n') count++;
    }
    if (count == 0) return NULL;

    // Pick a random sentence index
    size_t target = rand() % count;

    // Iterate again to find it
    const char* start = text;
    const char* end = NULL;
    size_t index = 0;
    for (const char* p = text; ; p++) {
        if (*p == '\n' || *p == '\0') {
            if (index == target) {
                end = p;
                break;
            }
            start = p + 1;
            index++;
        }
        if (*p == '\0') break;
    }

    // Allocate a temporary buffer to return a null-terminated string
    size_t len = end - start;
    char* sentence = (char*)malloc(len + 1);
    if (!sentence) return NULL;

    memcpy(sentence, start, len);
    sentence[len] = '\0';
    return sentence;
}