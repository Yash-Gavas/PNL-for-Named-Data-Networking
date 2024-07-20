const express = require('express');
const bodyParser = require('body-parser');
const fs = require('fs');

const app = express();
const PORT = 3000;

const namePrefixTree = {
    root: {
        name: null,
        children: {},
    },
};

const accessProbabilities = {
    probabilities: Array.from({ length: 26 }, (_, i) => 0.50),
};

let server;

app.use(bodyParser.json());
app.use(express.static('public'));

app.post('/execute', (req, res) => {
    const choice = req.body.choice;
    const name = req.body.name;

    switch (choice) {
        case 1:
            insertName(name);
            res.json({ output: `Name '${name}' inserted successfully.` });
            break;
        case 2:
            const isFound = lookupName(name);
            res.json({ output: `Name '${name}' ${isFound ? 'found' : 'not found'}.` });
            break;
        case 3:
            const deleteResult = deleteName(name);
            res.json({ output: deleteResult });
            break;
        case 4:
            const nptOutput = printNPT();
            const acceptHeader = req.get('Accept');

            if (acceptHeader && acceptHeader.includes('text/html')) {
                // Send HTML response with NPT tree
                res.send(`
                    <!DOCTYPE html>
                    <html lang="en">
                    <head>
                        <meta charset="UTF-8">
                        <meta name="viewport" content="width=device-width, initial-scale=1.0">
                        <title>Name Prefix Tree (NPT)</title>
                    </head>
                    <body>
                        <pre>${nptOutput}</pre>
                    </body>
                    </html>
                `);
            } else {
                // Send JSON response with NPT tree
                res.json({ output: nptOutput });
            }
            break;
        case 5:
            const accessProbOutput = printAccessProbabilities();
            res.json({ output: accessProbOutput });
            break;
        case 6:
            const avgAccessProbOutput = printAverageAccessProbability();
            res.json({ output: avgAccessProbOutput });
            break;
        case 7:
            saveNPTToFile();
            res.json({ output: 'Name Prefix Tree (NPT) saved to file successfully.' });
            break;
        case 8:
            exitServer();
            res.json({ output: 'Server is shutting down.' });
            break;
        default:
            res.status(400).json({ error: 'Invalid choice.' });
    }
});

function insertName(name) {
    // Your logic to insert the name into the data structure (e.g., Name Prefix Tree)
    // In this example, we are using a simple object with children.
    let current = namePrefixTree.root;
    for (let i = 0; i < name.length; i++) {
        const char = name[i];
        if (!current.children[char]) {
            current.children[char] = { name: null, children: {} };
        }
        current = current.children[char];
    }
    current.name = name;
}

function lookupName(name) {
    // Your logic to lookup the name in the data structure
    // In this example, we are using a simple object with children.
    let current = namePrefixTree.root;
    for (let i = 0; i < name.length; i++) {
        const char = name[i];
        if (!current.children[char]) {
            return false; // Name not found
        }
        current = current.children[char];
    }
    return current.name !== null;
}

function deleteName(name) {
    // Your logic to delete the name from the data structure
    // In this example, we are using a simple object with children.
    let current = namePrefixTree.root;
    for (let i = 0; i < name.length; i++) {
        const char = name[i];
        if (!current.children[char]) {
            // Name not found for deletion
            console.log(`Node '${name}' not found for deletion.`);
            return `Name '${name}' not found for deletion.`;
        }
        current = current.children[char];
    }

    if (current.name === null) {
        // Name not found for deletion
        console.log(`Name '${name}' not found for deletion.`);
        return `Name '${name}' not found for deletion.`;
    }

    current.name = null;
    return `Name '${name}' deleted successfully.`;
}

function printNPT() {
    // Your logic to traverse the NPT and generate the formatted output
    const result = [];
    traverseNPT(namePrefixTree.root, "", result);
    return result.join('\n');
}

function traverseNPT(node, prefix, result) {
    if (node.name) {
        result.push(`${prefix} (${node.name})`);
    }
    for (const char in node.children) {
        traverseNPT(node.children[char], prefix + char, result);
    }
}

function printAccessProbabilities() {
    const probabilities = calculateAccessProbabilities(namePrefixTree.root);
    const output = [];

    output.push("\nAccess Probabilities:");
    for (const char in probabilities) {
        output.push(`${char}: ${probabilities[char].toFixed(2)}`);
    }

    return output.join('\n');
}

function calculateAccessProbabilities(node) {
    const probabilities = {};
    traverseProbabilities(node, probabilities);
    return probabilities;
}

function traverseProbabilities(node, probabilities) {
    if (node.name) {
        for (let i = 0; i < node.name.length; i++) {
            const char = node.name[i].toLowerCase();
            probabilities[char] = (probabilities[char] || 0) + 0.1;
        }
    }
    for (const char in node.children) {
        traverseProbabilities(node.children[char], probabilities);
    }
}

function printAverageAccessProbability() {
    // Your logic to calculate and print average access probability
    const sum = accessProbabilities.probabilities.reduce((acc, prob) => acc + prob, 0);
    const average = sum / accessProbabilities.probabilities.length;
    return `Average Access Probability: ${average.toFixed(2)}`;
}

function saveNPTToFile() {
    const nptOutput = printNPT();
    fs.writeFileSync('npt_output.txt', nptOutput);
}

function exitServer() {
    if (server) {
        server.close(() => {
            console.log('Server is shutting down.');
            process.exit(0);
        });
    } else {
        console.log('Server is not running.');
        process.exit(0);
    }
}

server = app.listen(PORT, () => {
    console.log(`Server is running at http://localhost:${PORT}`);
});
