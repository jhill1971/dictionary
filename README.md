# Dictionary Command Line Tool

The Dictionary Command Line Tool is a simple program that allows users to look up word definitions using the [DictionaryAPI](https://dictionaryapi.dev/) service. It retrieves dictionary data from the API based on the provided word and displays the top definition in a structured format.

## Features

- Look up definitions for English words.
- Display the top definition for each word in a structured format.
- Supports nouns, verbs, adjectives, and other parts of speech.

## Installation

1. Clone the repository:

    ```bash
    git clone https://github.com/your-username/dictionary-cli.git
    ```

2. Navigate to the project directory:

    ```bash
    cd dictionary-cli
    ```

3. Compile the program:

    ```bash
    make
    ```

## Usage

To use the Dictionary Command Line Tool, follow these steps:

1. Run the executable with the word you want to look up as the argument:

    ```bash
    ./dict <word>
    ```

    Replace `<word>` with the word you want to look up.

2. The program will retrieve the top definition for the word and display it in the following format:

    ```
    Word: <Word>
    Part of Speech: <Part of Speech>
    Definition: <Definition>
    ```

3. If there are multiple definitions for the word, only the top definition will be displayed.

## Dependencies

- [libcurl](https://curl.se/libcurl/)
- [json-c](https://github.com/json-c/json-c)

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgements

- [DictionaryAPI](https://dictionaryapi.dev/) for providing the dictionary data API.

- 


https://github.com/jhill1971/dictionary/assets/60713038/815b96c6-be62-489c-a1ef-dcc901a57625

