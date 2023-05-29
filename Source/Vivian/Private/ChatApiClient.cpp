// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatApiClient.h"

#include "Interfaces/IHttpResponse.h"

UChatApiClient::UChatApiClient(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    Http = &FHttpModule::Get();
}


// Make an API request
void UChatApiClient::SendOpenAIChatRequest(const FString& InputText)
{
    // Create a new HTTP request object
    const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http-> CreateRequest();

    // Set the callback function to handle the response
    Request->OnProcessRequestComplete().BindUObject(this, &UChatApiClient::HandleAPIResponse);

    // Set the URL
    Request->SetURL(ChatApiUrl);

    // Set the HTTP verb (such as GET, POST, PUT, DELETE)
    Request->SetVerb("POST");

    // Set headers needed for the request
    Request->SetHeader("User-Agent", "X-UnrealEngine-Agent");
    Request->SetHeader("Content-Type", "application/json");
    Request->SetHeader("Authorization", "Bearer " + ApiKey);

    // create a JSON object
    const TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

    // Add the "model" key-value pair to the JSON object
    JsonObject->SetStringField("model", "gpt-3.5-turbo");

    // Create a new JSON array for the "messages" key-value pair
    TArray<TSharedPtr<FJsonValue>> MessagesArray;

    // Message object with the base conversation context
    const TSharedPtr<FJsonObject> BaseConversationContextMessageObject = MakeShareable(new FJsonObject);
    BaseConversationContextMessageObject->SetStringField("role", "system");
    BaseConversationContextMessageObject->SetStringField("content", BaseConversationContext);
    MessagesArray.Add(MakeShareable(new FJsonValueObject(BaseConversationContextMessageObject)));

    // Message object with the user input
    const TSharedPtr<FJsonObject> MessageObject = MakeShareable(new FJsonObject);
    MessageObject->SetStringField("role", "user");
    MessageObject->SetStringField("content", InputText);
    MessagesArray.Add(MakeShareable(new FJsonValueObject(MessageObject)));

    // Add the JSON array to the JSON object
    JsonObject->SetArrayField("messages", MessagesArray);

    // convert the JSON object to a string
    FString JsonString;
    const TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);
    
    // Set the content of the request
    Request->SetContent(TArray(reinterpret_cast<const uint8*>(TCHAR_TO_UTF8(*JsonString)), JsonString.Len()));

    // Send the HTTP request
    Request->ProcessRequest();
}


void UChatApiClient::HandleAPIResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
{
    // Check if the request was successful
    if (bSuccess && Response.IsValid() && Response->GetResponseCode() == EHttpResponseCodes::Ok)
    {
        // Get the response body as a string
        const FString ResponseBody = Response->GetContentAsString();

        // Create a new Json Object
        TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

        // Create a reader for the response body
        const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<TCHAR>::Create(ResponseBody);

        // Check if reading is succesful
        if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
        {
            // Log the JSON object as a string
            FString JsonObjectString;
            const TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonObjectString);
            FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);

            // Extract the "choices" array from the JSON object
            const TArray<TSharedPtr<FJsonValue>>* ChoicesArray = nullptr;

            // Check if the "choices" key exists in the JSON object
            if (JsonObject->TryGetArrayField("choices", ChoicesArray) && ChoicesArray->Num() > 0)
            {
                const TSharedPtr<FJsonObject> FirstChoiceObject = (*ChoicesArray)[0]->AsObject();
                const TSharedPtr<FJsonObject> MessageObject = FirstChoiceObject->GetObjectField("message");

                // Check if the "role" and "content" keys exist in the JSON object
                if (FString Role, Content; MessageObject->TryGetStringField("role", Role) && MessageObject->
                    TryGetStringField("content", Content))
                {
                    UE_LOG(LogTemp, Display, TEXT("Role: %s, Content: %s"), *Role, *Content);
                    // Broadcast the response text
                    OnChatResponseProcessed.Broadcast(Content);
                }
            }
            // If the "choices" key does not exist in the JSON object
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("No choices array found"));
            }
        }
    }
    else
    {
        // Print the error message to the console for debugging
        UE_LOG(LogTemp, Error, TEXT("API request failed: %s"), *Response->GetContentAsString());
        // Broadcast a generic error message
        OnChatResponseProcessed.Broadcast("Er is iets fout gegaan. Probeer het later opnieuw.");
    }
}