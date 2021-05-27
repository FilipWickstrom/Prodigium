#include "MainMenu.h"

const bool MainMenu::SetUpBuffer()
{
	HRESULT hr, hr2;

	DirectX::SimpleMath::Vector3 eye = { this->eyePos.x, this->eyePos.y, this->eyePos.z };
	DirectX::SimpleMath::Matrix view = DirectX::XMMatrixLookToLH(eye, { 0.0f, eye.y - 23.0f, this->eyePos.z + 50.0f }, { 0.0f, 1.0f, 0.0f });

	DirectX::SimpleMath::Matrix proj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PI * 0.45f, (float)(Graphics::GetWindowWidth() / Graphics::GetWindowHeight()), 0.1f, 400.0f);
	this->menuView = view.Transpose();
	this->menuProj = proj.Transpose();

	Package pack;

	pack.view = this->menuView;
	pack.projection = this->menuProj;

	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.ByteWidth = sizeof(Package);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = &pack;

	hr = Graphics::GetDevice()->CreateBuffer(&desc, &data, &this->viewBuffer);
	if (FAILED(hr))
		return false;
	FogBuffer fog;
	fog.viewMatrix = this->menuView;
	fog.cameraPos = this->eyePos;
	fog.fogColour = { 0.2f,0.2f,0.2f,0.f };
	fog.fogRange = 200.f;
	fog.fogStart = 10.f;
	desc.ByteWidth = sizeof(FogBuffer);
	data.pSysMem = &fog;
	hr2 = Graphics::GetDevice()->CreateBuffer(&desc, &data, &this->eyeBuffer);
	if (FAILED(hr2))
		return false;

	return !FAILED(hr2);
}

MainMenu::MainMenu()
{
	this->viewBuffer = nullptr;
	this->eyeBuffer = nullptr;
	this->inMenu = true;
	this->menuView = {};
	this->time = 0;
	this->distToGoal = 99999;

	// Start pos
	this->eyePos = { 0.0f, 5.0f, -1.0f, 1.0f };
}

MainMenu::~MainMenu()
{
	if (this->viewBuffer)
		this->viewBuffer->Release();

	if (this->eyeBuffer)
		this->eyeBuffer->Release();
}

const bool MainMenu::IsInMenu() const
{
	return this->inMenu;
}

void MainMenu::Init()
{
	if (!this->SetUpBuffer())
		std::cout << "Setting up buffer failed!\n";
}

void MainMenu::Switch(bool boolean)
{
	this->inMenu = boolean;
	this->time = 0;
	this->distToGoal = 99999;
}

void MainMenu::ZoomIn(DirectX::SimpleMath::Vector4 endPos, const float& deltaTime, bool& inGoal)
{
	this->time += deltaTime;
	DirectX::SimpleMath::Vector4 result;
	DirectX::SimpleMath::Vector4::Lerp(this->eyePos, endPos, this->time, result);

	this->distToGoal = (result - endPos).Length();
	if (this->distToGoal < 0)
		this->distToGoal *= -1;

	if (this->distToGoal < 5.0f)
		inGoal = true;


	DirectX::SimpleMath::Vector3 eye = { result.x, result.y, result.z };
	DirectX::SimpleMath::Matrix view = DirectX::XMMatrixLookToLH(eye, { 0.0f, eye.y + 0.00001f, eye.z + 50.0f }, { 0.0f, 1.0f, 0.0f });

	DirectX::SimpleMath::Matrix proj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PI * 0.5f, (float)(Graphics::GetWindowWidth() / Graphics::GetWindowHeight()), 0.1f, 400.0f);
	this->menuView = view.Transpose();
	this->menuProj = proj.Transpose();
	Package pack;

	pack.view = this->menuView;
	pack.projection = this->menuProj;
	D3D11_MAPPED_SUBRESOURCE submap;
	HRESULT hr = Graphics::GetContext()->Map(this->viewBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &submap);
	memcpy(submap.pData, &pack, sizeof(Package));

	Graphics::GetContext()->Unmap(this->viewBuffer, 0);
}

void MainMenu::Reset()
{
	DirectX::SimpleMath::Vector3 eye = { 0, 5, 0 };
	DirectX::SimpleMath::Matrix view = DirectX::XMMatrixLookToLH(eye, { 0.0f, eye.y - 23.0f, eye.z + 50.0f }, { 0.0f, 1.0f, 0.0f });

	DirectX::SimpleMath::Matrix proj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PI * 0.45f, (float)(Graphics::GetWindowWidth() / Graphics::GetWindowHeight()), 0.1f, 400.0f);
	this->menuView = view.Transpose();
	this->menuProj = proj.Transpose();
	Package pack;

	pack.view = this->menuView;
	pack.projection = this->menuProj;
	D3D11_MAPPED_SUBRESOURCE submap;
	HRESULT hr = Graphics::GetContext()->Map(this->viewBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &submap);
	memcpy(submap.pData, &pack, sizeof(Package));

	Graphics::GetContext()->Unmap(this->viewBuffer, 0);
}

void MainMenu::Update()
{
	Graphics::GetContext()->VSSetConstantBuffers(0, 1, &this->viewBuffer);
	Graphics::GetContext()->PSSetConstantBuffers(1, 1, &this->eyeBuffer);
	Graphics::GetContext()->GSSetConstantBuffers(0, 1, &this->viewBuffer);
	Graphics::GetContext()->GSSetConstantBuffers(1, 1, &this->eyeBuffer);
}
