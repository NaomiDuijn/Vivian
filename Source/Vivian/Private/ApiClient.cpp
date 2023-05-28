// Fill out your copyright notice in the Description page of Project Settings.
#include "ApiClient.h"

UApiClient::UApiClient(const FObjectInitializer& ObjectInitializer)
    : UUserWidget(ObjectInitializer)
{
    Http = &FHttpModule::Get();
}

FString UApiClient::GetResponseText() const
{
    return ResponseText;
}

void UApiClient::SetResponseText(const FString& Response)
{
    // Let the user know that it's done loading
    LoadingSpinner->SetVisibility(ESlateVisibility::Collapsed);
    ResponseText = Response;
    OnResponseProcessed.Broadcast();
    ApiTextOutput->SetText(FText::FromString(Response));
    UE_LOG(LogTemp, Display, TEXT("Set responsetext to: %s"), *Response);
}

// Make an API request
void UApiClient::SendOpenAIChatRequest(const FString& InputText)
{
    // Create a new HTTP request object
    const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http-> CreateRequest();

    // Set the callback function to handle the response
    Request->OnProcessRequestComplete().BindUObject(this, &UApiClient::HandleAPIResponse);

    // Set the URL
    Request->SetURL(ChatApiUrl);

    // Set the HTTP verb (such as GET, POST, PUT, DELETE)
    Request->SetVerb("POST");

    // Set headers needed for the request
    Request->SetHeader("User-Agent", "X-UnrealEngine-Agent");
    Request->SetHeader("Content-Type", "application/json"); //;q=0.9,text/plain
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


void UApiClient::HandleAPIResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
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
                    // Set the response text to the "content" value
                    SetResponseText(Content);
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
        // Set the response text to the error message
        SetResponseText(*Response->GetContentAsString());
    }
}

FString UApiClient::AddStringValueToMultipartFormData(FString Name, FString Value) {
    return FString(TEXT("\r\n"))
        + BoundaryBegin
        + FString(TEXT("Content-Disposition: form-data; name=\""))
        + Name
        + FString(TEXT("\"\r\n\r\n"))
        + Value;
}

void UApiClient::SendAudioToOpenAI()
{
    // Let the user know that it's loading
    LoadingSpinner->SetVisibility(ESlateVisibility::Visible);
    // Create a new HTTP request object
    const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

    // Set the URL
    Request->SetURL(TranscriptionApiUrl);

    // Set the HTTP verb (such as GET, POST, PUT, DELETE)
    Request->SetVerb("POST");
    
    // Explanation of a boundary label: https://roytuts.com/boundary-in-multipart-form-data/
    // Create a boundary label, for the header
    BoundaryLabel = FString(TEXT("e543322540af456f9a3773049ca02529-")) + FString::FromInt(FMath::Rand());
    // Boundary label for beginning of every payload chunk 
    BoundaryBegin = FString(TEXT("--")) + BoundaryLabel + FString(TEXT("\r\n"));
    // Boundary label for the end of payload
    BoundaryEnd = FString(TEXT("\r\n--")) + BoundaryLabel + FString(TEXT("--\r\n"));

    // Set headers or parameters needed for the request
    Request->SetHeader("User-Agent", "X-UnrealEngine-Agent");
    Request->SetHeader("Authorization", "Bearer " + ApiKey);
    // Set the content type of the request with the boundary label
    Request->SetHeader(TEXT("Content-Type"), FString(TEXT("multipart/form-data; boundary=")) + BoundaryLabel);
    // Set the file path
    const FString FilePath = FPaths::ProjectSavedDir() + TEXT("BouncedWavFiles/UserRecording.wav");

    // Create the multipart/form-data content
    TArray<uint8> CombinedContent;
    TArray<uint8> FileRawData;
    if (!FFileHelper::LoadFileToArray(FileRawData, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load file: %s"), *FilePath);
        return;
    }
    // Get the file name and content type
    const FString FileName = FPaths::GetCleanFilename(FilePath);
    const FString ContentType = TEXT("audio/wav");

    // Add the boundary for the file to the CombinedContent, which is different from text payload
    const FString FileBoundaryString = FString(TEXT("\r\n"))
        + BoundaryBegin
        + FString(TEXT("Content-Disposition: form-data; name=\"file\"; filename=\""))
        + FileName + "\"\r\n"
        + "Content-Type: " + ContentType
        + FString(TEXT("\r\n\r\n"));
    CombinedContent.Append(reinterpret_cast<const uint8*>(TCHAR_TO_UTF8(*FileBoundaryString)), FileBoundaryString.Len());
    // Add the actual file data
    CombinedContent.Append(FileRawData);
    // Add the boundary for the text payload with the model information
    const FString ModelData = AddStringValueToMultipartFormData("model", "whisper-1");
    CombinedContent.Append(reinterpret_cast<const uint8*>(TCHAR_TO_UTF8(*ModelData)), ModelData.Len());
    // Add the boundary for the text payload with the language information
    const FString LangData = AddStringValueToMultipartFormData("language", "nl");
    CombinedContent.Append(reinterpret_cast<const uint8*>(TCHAR_TO_UTF8(*LangData)), LangData.Len());
    // Add the closing boundary
    CombinedContent.Append(reinterpret_cast<const uint8*>(TCHAR_TO_UTF8(*BoundaryEnd)), BoundaryEnd.Len());
    // Set the content for the request
    Request->SetContent(CombinedContent);
    // Send the request
    Request->OnProcessRequestComplete().BindUObject(this, &UApiClient::OnTranscriptionComplete);
    Request->ProcessRequest();
}

void UApiClient::OnTranscriptionComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
{
    if (!bSuccess || !Response.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to transcribe audio"));
        return;
    }
    // Print the response to the console for debugging
    const FString TranscriptionResponseText = Response->GetContentAsString();
    UE_LOG(LogTemp, Log, TEXT("Transcription response: %s"), *TranscriptionResponseText);
    
    // Deserialize the JSON response
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
    if (const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<TCHAR>::Create(TranscriptionResponseText); FJsonSerializer::Deserialize(JsonReader, JsonObject))
    {
        if (FString Transcription; JsonObject->TryGetStringField("text", Transcription))
        {
            UE_LOG(LogTemp, Log, TEXT("Transcription: %s"), *Transcription);
            // Check if the transcription is empty
            if (Transcription.IsEmpty())
            {
                UE_LOG(LogTemp, Warning, TEXT("Transcription is empty"));
                return;
            }
            // Send the input text to the OpenAI API
            SendOpenAIChatRequest(Transcription);
        }
    }
}
