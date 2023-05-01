// Fill out your copyright notice in the Description page of Project Settings.
#include "ApiClient.h"

UApiClient::UApiClient(const FObjectInitializer& ObjectInitializer)
    : UUserWidget(ObjectInitializer)
{
    Http = &FHttpModule::Get();
}

void UApiClient::SetResponseText(FString Response)
{
    ApiTextOutput->SetText(FText::FromString(Response));
    UE_LOG(LogTemp, Display, TEXT("Set text to: %s"), *Response);
}

void UApiClient::MakeAPIRequest()
{
    // Create a new HTTP request object
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http-> CreateRequest();
    FText InputFieldText = ApiTextInput->GetText();
    InputText = InputFieldText.ToString();
    // Set the callback function to handle the response
    Request->OnProcessRequestComplete().BindUObject(this, &UApiClient::HandleAPIResponse);
    // Set the URL of the API endpoint you want to call
    FString URL = "https://api.openai.com/v1/chat/completions";
    Request->SetURL(URL);

    // Set the HTTP verb (such as GET, POST, PUT, DELETE)
    Request->SetVerb("POST");

    // Set any headers or parameters needed for the request
    Request->SetHeader("User-Agent", "X-UnrealEngine-Agent");
    Request->SetHeader("Content-Type", "application/json"); //;q=0.9,text/plain
    Request->SetHeader("Authorization", "Bearer " + ApiKey);

    // create a JSON object
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

    // Add the "model" key-value pair to the JSON object
    JsonObject->SetStringField("model", "gpt-3.5-turbo");

    // Create a new JSON array for the "messages" key-value pair
    TArray<TSharedPtr<FJsonValue>> MessagesArray;

    TSharedPtr<FJsonObject> MessageObject = MakeShareable(new FJsonObject);
    MessageObject->SetStringField("role", "user");
    MessageObject->SetStringField("content", InputText);
    MessagesArray.Add(MakeShareable(new FJsonValueObject(MessageObject)));
    // Add the JSON array to the JSON object
    JsonObject->SetArrayField("messages", MessagesArray);

    // convert the JSON object to a string
    FString JsonString;
    TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);
    // Set the content of the request
    Request->SetContent(TArray<uint8>((const uint8*)TCHAR_TO_UTF8(*JsonString), JsonString.Len()));

    // Send the HTTP request
    Request->ProcessRequest();
}

void UApiClient::HandleAPIResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
{
    // Create the JsonObject
    TSharedPtr <FJsonObject> JsonObject;
    // Check if the request was successful
    if (bSuccess && Response.IsValid() && Response->GetResponseCode() == EHttpResponseCodes::Ok)
    {
        // Get the response body as a string
        const FString ResponseBody = Response->GetContentAsString();
        // Create a reader for the response body
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);

        // Check is reading is succesful
        if (FJsonSerializer::Deserialize(Reader, JsonObject))
        {
            // Parse the response and do something with the data
        }

        // Print the response body to the console for debugging
        UE_LOG(LogTemp, Warning, TEXT("API response body: %s"), *ResponseBody);
        UApiClient::SetResponseText(ResponseBody);
    }
    else
    {
        // Handle any errors or exceptions that occurred during the API call
        // ...

        // Print the error message to the console for debugging
        UE_LOG(LogTemp, Error, TEXT("API request failed: %s"), *Response->GetContentAsString());
        UApiClient::SetResponseText(*Response->GetContentAsString());
    }
}


