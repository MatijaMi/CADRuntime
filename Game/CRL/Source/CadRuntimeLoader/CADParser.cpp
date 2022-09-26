// Fill out your copyright notice in the Description page of Project Settings.


#include "CADParser.h"
#include "FFileMatch.h"

TArray<FString> UCADParser::ParseOBJ(FString Input, TArray<FVector>& Vertices, TArray<FVector2D>& TextureCoords, TArray<FVector>& Normals)
{
	TArray<FString> InputArray;
	FString Separator = "\n";
	FString LoopSeparator = " ";
	FString Output = "";
	int Scale = 2540;
	TArray<FString> Sections;
	TArray<FString> WorkingArray;
	FString CurrentMaterial;
	
	//Min and Max used for centering and scaling vertices
	float maxX = -FLT_MAX, maxY = -FLT_MAX, maxZ = -FLT_MAX;
	float minX = FLT_MAX, minY = FLT_MAX, minZ = FLT_MAX;

	//Parse loaded string file into an array of lines
	Input.ParseIntoArray(InputArray, *Separator);
	for (auto line : InputArray) {
		WorkingArray.Empty();
		//Vertices
		if (line.StartsWith("v ")) {
			line.RemoveFromStart("v ");
			line.ParseIntoArray(WorkingArray, *LoopSeparator);

			float x = FCString::Atof(*WorkingArray[0]);
			float y = FCString::Atof(*WorkingArray[1]);
			float z = FCString::Atof(*WorkingArray[2]);
			if (x > maxX) { maxX = x; };
			if (x < minX) { minX = x; };
			if (y > maxY) { maxY = y; };
			if (y < minY) { minY = y; };
			if (z > maxZ) { maxZ = z; };
			if (z < minZ) { minZ = z; };
			Vertices.Add(FVector(x, y, z));
		}
		else {
			//Vertex Texture Coordinates
			if (line.StartsWith("vt ")) {
				line.RemoveFromStart("vt ");
				line.ParseIntoArray(WorkingArray, *LoopSeparator);
				float x = FCString::Atof(*WorkingArray[0]);
				float y = FCString::Atof(*WorkingArray[1]);
				TextureCoords.Add(FVector2D(x, y));
			}
			else {
				//Vertex Normals
				if (line.StartsWith("vn ")) {
					line.RemoveFromStart("vn ");
					line.ParseIntoArray(WorkingArray, *LoopSeparator);
					float x = FCString::Atof(*WorkingArray[0]);
					float y = FCString::Atof(*WorkingArray[1]);
					float z = FCString::Atof(*WorkingArray[2]);
					Normals.Add(FVector(x, y, z));
				}
				else {
					//Group
					if (line.StartsWith("g ")) {
						if (Output != "") {
							Sections.Add(Output);
							Output = "";
						}
					}
					else {
						//Face
						if (line.StartsWith("f ")) {
							if (Output == "") {
								Output.Append(CurrentMaterial);
								Output.Append("\n");
							}
							TArray<FString> splitArray;
							line.ParseIntoArray(splitArray, *LoopSeparator);
							//Polygon to Triangle Fan transformation
							if (splitArray.Num() > 4) {
								for (int i = 0; i < splitArray.Num() - 3; i++) {
									Output.Append("f " + splitArray[1] + " " + splitArray[i+2] + " " + splitArray[i+3]);
									Output.Append("\n");
								}

							}else {
								Output.Append(line);
								Output.Append("\n");
							}
							
						}
						else {
							//Material
							if (line.StartsWith("usemtl ")) {
								Output.Append(line);
								Output.Append("\n");
								CurrentMaterial = line;
							}
						}
					}
				}
			}
		}
	}

	float maxDiff = abs(maxX - minX);
	if (abs(maxY - minY) > maxDiff) {
		maxDiff = abs(maxY - minY);
	}
	else {
		if (abs(maxZ - minZ) > maxDiff) {
			maxDiff = abs(maxZ - minZ);
		}
	}
	//Centering vertices and scaling them to useable size
	for (auto& vertex : Vertices) {
		vertex.X = -(2 * ((vertex.X - minX) / (maxX - minX)) - 1) * (Scale *(abs(maxX - minX) / maxDiff));
		vertex.Y = (2 * ((vertex.Y - minY) / (maxY - minY)) - 1) * (Scale * (abs(maxY - minY) / maxDiff));
		vertex.Z = (2 * ((vertex.Z - minZ) / (maxZ - minZ)) - 1) * (Scale * (abs(maxZ - minZ) / maxDiff));
	}

	Sections.Add(Output);
	return Sections;
}

TMap<FString, FString>  UCADParser::ParseOBJMaterial(FString input)
{
	TArray<FString> inputArray;
	FString sep = "\n";
	input.ParseIntoArray(inputArray, *sep);
	FString output = "";
	TMap<FString, FString> Materials;
	FString Values;
	FString LastKey;

	for (auto line : inputArray) {
		//Name
		if (line.StartsWith("newmtl ")) {
			line.RemoveFromStart("newmtl ");
			if (Values != "") {
				Materials.Add(LastKey, Values);
				Values = "";
			}
			LastKey = line;
		}
		else {
			//Diffuse Colour
			if (line.StartsWith("Kd ")) {
				line.RemoveFromStart("Kd ");
				Values.Append(line + " ");
			}
			else {
				//Opacity
				if (line.StartsWith("d ")) {
					line.RemoveFromStart("d ");
					Values.Append(line + " ");
				}
				else {
					//Specular Exponent
					if (line.StartsWith("Ns ")) {
						line.RemoveFromStart("Ns ");
						Values.Append(line + " ");
					}
				}

			}
		}
	}

	Materials.Add(LastKey, Values);
	return Materials;
}



FString UCADParser::LoadFileToString(FString Filename) {
	FString directory;
	FString name;
	Filename.Split("\\", &directory, &name, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	FString result;
	IPlatformFile& file = FPlatformFileManager::Get().GetPlatformFile();

	if (file.CreateDirectory(*directory)) {
		FString myFile = directory + "\\" + name;
		FFileHelper::LoadFileToString(result, *myFile);
	}
	return result;

}

TArray<FString> UCADParser::OpenFolder(FString Directory) {
	FString result;
	IPlatformFile& file = FPlatformFileManager::Get().GetPlatformFile();
	TArray<FString> FolderContent;
	TArray<FString> FilteredContent;
	if (file.CreateDirectory(*Directory)) {
		FFileMatch FileMatch(FolderContent, FString(".obj"), true, true);
		file.IterateDirectory(*Directory, FileMatch);
	}
	for (auto& name : FolderContent) {
		if (name.StartsWith("\\")) {
			FilteredContent.Add(name);
		}
		else {
			if (name.EndsWith(".obj")|| name.EndsWith(".mtl") || name.EndsWith(".stl")) {
				FilteredContent.Add(name);
			}
		}
	}
	return FilteredContent;

}

TArray<FString> UCADParser::ParseFile(FString Input, TArray<FVector>& Vertices, TArray<FVector2D>& TextureCoords, TArray<FVector>& Normals)
{
	if (Input.StartsWith("solid")) {
		return ParseSTL(Input, Vertices, TextureCoords, Normals);
	}
	else {
		return ParseOBJ(Input, Vertices, TextureCoords, Normals);
	}
}


TArray<FString> UCADParser::ParseSTL(FString Input, TArray<FVector>& Vertices, TArray<FVector2D>& textureCoords, TArray<FVector>& Normals)
{

	TArray<FString> inputArray;
	FString sep = "\n";
	Input.ParseIntoArray(inputArray, *sep);
	TArray<FString> Sections;
	FString output = "";
	TArray<FString> workingArray;
	int AddedTriangles =0;
	FString loopSeparator = " ";
	float maxX = -FLT_MAX, maxY = -FLT_MAX, maxZ = -FLT_MAX;
	float minX = FLT_MAX, minY = FLT_MAX, minZ = FLT_MAX;

	for (auto line : inputArray) {
		workingArray.Empty();
		//Vertex
		if (line.Contains("vertex")) {
			line =line.TrimStart();
			line.RemoveFromStart("vertex ");
			line.ParseIntoArray(workingArray, *loopSeparator);
			
			float x = FCString::Atof(*workingArray[0]);
			float y = FCString::Atof(*workingArray[1]);
			float z = FCString::Atof(*workingArray[2]);
			if (x > maxX) { maxX = x; }
			if (x < minX) { minX = x; }
			if (y > maxY) { maxY = y; }
			if (y < minY) { minY = y; }
			if (z > maxZ) { maxZ = z; }
			if (z < minZ) { minZ = z; }
			Vertices.Add(FVector(x, y, z));
		}
		else {
			//Triangular Face
			if (line.Contains("outer loop")) {
				int t = AddedTriangles;
				FString x,y,z = "";
				x = x.FromInt(t * 3+1);
				y = y.FromInt(t * 3 +2);
				z = z.FromInt(t * 3 +3);
				output.Append("f " + x + " " + y + " " + z);
				output.Append("\n");
				AddedTriangles++;
			}
		}
		
	}
	// Centering and Scaling
	float maxDiff = abs(maxX - minX);
	if (abs(maxY - minY) > maxDiff) {
		maxDiff = abs(maxY - minY);
	}
	else {
		if (abs(maxZ - minZ) > maxDiff) {
			maxDiff = abs(maxZ - minZ);
		}
	}

	for (auto& vertex : Vertices) {
		vertex.X = -(2 * ((vertex.X - minX) / (maxX - minX)) - 1) * (2540 * (abs(maxX - minX) / maxDiff));
		vertex.Y = (2 * ((vertex.Y - minY) / (maxY - minY)) - 1) * (2540 * (abs(maxY - minY) / maxDiff));
		vertex.Z = (2 * ((vertex.Z - minZ) / (maxZ - minZ)) - 1) * (2540 * (abs(maxZ - minZ) / maxDiff));
	}
	Sections.Add(output);
	return Sections;
}