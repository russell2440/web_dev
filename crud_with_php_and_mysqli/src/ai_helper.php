<?php

require_once 'flash_helper.php';

/**
 * Fetches a dynamically generated list of players from the Gemini API.
 * 
 * @return array Array of [firstname, lastname] pairs.
 */
function generate_dynamic_players(): array {
    $apiKey = getenv('GEMINI_API_KEY');
    
    if (!$apiKey) {
        return [
            ['Fallback', 'Player1'],
            ['Fallback', 'Player2']
        ];
    }

    $count = rand(80, 120);

    //$url = "https://generativelanguage.googleapis.com/v1beta/models/gemini-3.6-flash:generateContent?key=" . trim($apiKey);
    $url = "https://generativelanguage.googleapis.com/v1beta/models/gemini-3.5-flash-lite:generateContent?key=" . trim($apiKey);

    $promptText = "Generate a list of exactly {$count} realistic American first and last names commonly used between the 1950s and 1980s.";

    $payload = [
        "contents" => [
            [
                "parts" => [
                    ["text" => $promptText]
                ]
            ]
        ],
        "generationConfig" => [
            "responseMimeType" => "application/json",
            "responseSchema" => [
                "type" => "OBJECT",
                "properties" => [
                    "players" => [
                        "type" => "ARRAY",
                        "items" => [
                            "type" => "OBJECT",
                            "properties" => [
                                "firstname" => ["type" => "STRING"],
                                "lastname"  => ["type" => "STRING"]
                            ],
                            "required" => ["firstname", "lastname"]
                        ]
                    ]
                ],
                "required" => ["players"]
            ]
        ]
    ];

    $maxRetries = 3;
    $attempt = 0;
    $httpCode = 0;
    $response = '';
    $curlError = '';

    set_time_limit(60); // Ensure PHP script won't crash mid-request

    while ($attempt < $maxRetries) {
        $attempt++;
        
        $ch = curl_init($url);
        curl_setopt_array($ch, [
            CURLOPT_RETURNTRANSFER => true,
            CURLOPT_POST           => true,
            CURLOPT_HTTPHEADER     => ['Content-Type: application/json'],
            CURLOPT_POSTFIELDS     => json_encode($payload),
            CURLOPT_SSL_VERIFYPEER => false,
            CURLOPT_SSL_VERIFYHOST => 0,
            CURLOPT_CONNECTTIMEOUT => 15, // 15s to establish connection
            CURLOPT_TIMEOUT        => 45  // 45s total execution window
        ]);

        $response = curl_exec($ch);
        $curlError = curl_error($ch);
        $httpCode  = curl_getinfo($ch, CURLINFO_HTTP_CODE);
        curl_close($ch);

        // If successful, break out of retry loop
        if ($httpCode === 200 && !$curlError) {
            break;
        }

        // Only sleep and retry if the server was temporarily overloaded (503)
        if ($httpCode === 503) {
            sleep(1); 
        } else {
            // Fail fast on 429 (Rate Limit Exceeded), 400, 401, 404, etc.
            break;
        }
    }

    // Write trace to log
    file_put_contents(__DIR__ . '/gemini_debug.log', "Attempts: {$attempt}\nHTTP Code: {$httpCode}\ncURL Error: {$curlError}\nResponse:\n{$response}\n");

    // Fallback check if cURL fails, times out, or returns non-200 HTTP response
    if ($curlError || $httpCode !== 200 || empty($response)) {
        return [
            ['Fallback', 'Player1'],
            ['Fallback', 'Player2']
        ];
    }

    $responseData = json_decode($response, true);
    $rawText = $responseData['candidates'][0]['content']['parts'][0]['text'] ?? null;

    if (!$rawText) {
        return [
            ['Fallback', 'Player1'],
            ['Fallback', 'Player2']
        ];
    }

    $cleanJson = preg_replace('/^```(?:json)?\s*|\s*```$/i', '', trim($rawText));
    $parsedData = json_decode($cleanJson, true);

    $playersList = $parsedData['players'] ?? [];
    $formattedPlayers = [];

    foreach ($playersList as $p) {
        if (isset($p['firstname'], $p['lastname'])) {
            $formattedPlayers[] = [$p['firstname'], $p['lastname']];
        }
    }

    return !empty($formattedPlayers) ? $formattedPlayers : [
        ['Fallback', 'Player1'],
        ['Fallback', 'Player2']
    ];
}
