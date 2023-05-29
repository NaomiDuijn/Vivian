// Fill out your copyright notice in the Description page of Project Settings.


#include "TranscriptionApiClient.h"

#include "Interfaces/IHttpResponse.h"

UTranscriptionApiClient::UTranscriptionApiClient(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
	Http = &FHttpModule::Get();
}


FString UTranscriptionApiClient::AddStringValueToMultipartFormData(FString Name, FString Value) {
    return FString(TEXT("\r\n"))
        + BoundaryBegin
        + FString(TEXT("Content-Disposition: form-data; name=\""))
        + Name
        + FString(TEXT("\"\r\n\r\n"))
        + Value;
}

void UTranscriptionApiClient::SendAudioToOpenAI()
{
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
    Request->OnProcessRequestComplete().BindUObject(this, &UTranscriptionApiClient::OnTranscriptionComplete);
    Request->ProcessRequest();
}

void UTranscriptionApiClient::OnTranscriptionComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
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
            // Broadcast the transcription
            OnTranscriptionResponseProcessed.Broadcast(Transcription);
        }
    }
}