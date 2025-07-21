#pragma once
#include <unordered_map>
#include <string>
#include "AnimationClip.h"
#include "Transform.h"

using namespace std;

class Object
{
public:
	using ObjectId = unsigned int;
	explicit Object(ObjectId id) : id(id) {}
	ObjectId GetId() const { return id; }

private:
	ObjectId id;
	string name;
};

class ObjectManager
{
	Object::ObjectId nextId = 0;

	unordered_map<Object::ObjectId, Object> objects;

	unordered_map<Object::ObjectId, SpriteAnimator> spriteAnimators;
	unordered_map<Object::ObjectId, wstring> objectJsonPaths;
	unordered_map<Object::ObjectId, Transform> transforms;

public:
	Object::ObjectId CreateObject();
	void DestroyObject(Object::ObjectId id);
	Object* GetObject(Object::ObjectId id);
	unsigned int GetObjectCount() const { return nextId; }

	SpriteAnimator* GetSpriteAnimator(Object::ObjectId id);
	void AddSpriteAnimator(Object::ObjectId id, const SpriteAnimator& animator);

	void SetObjectJsonPath(Object::ObjectId id, const std::wstring& path);
	wstring GetObjectJsonPath(Object::ObjectId id) const;

	Transform* GetTransform(Object::ObjectId id);
	void AddTransform(Object::ObjectId id, const Transform& transform);
};