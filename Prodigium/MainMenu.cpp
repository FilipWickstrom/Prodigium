#include "MainMenu.h"

const bool MainMenu::SetUpBuffer()
{
	HRESULT hr, hr2;

	DirectX::SimpleMath::Vector3 eye = { this->eyePos.x, this->eyePos.y, this->eyePos.z };
	DirectX::SimpleMath::Matrix view = DirectX::XMMatrixLookToLH(eye, { 0.0f, eye.y + 0.00001f, this->eyePos.z + 50.0f }, { 0.0f, 1.0f, 0.0f });

	DirectX::SimpleMath::Matrix proj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PI * 0.5f, (float)(Graphics::GetWindowWidth() / Graphics::GetWindowHeight()), 0.1f, 400.0f);
	this->menuView = view.Transpose();
	this->menuProj = proj.Transpose();

	struct Package
	{
		DirectX::SimpleMath::Matrix view;
		DirectX::SimpleMath::Matrix projection;
	}pack;

	pack.view = this->menuView;
	pack.projection = this->menuProj;

	D3D11_BUFFER_DESC desc;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.ByteWidth = sizeof(Package);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &pack;

	hr = Graphics::GetDevice()->CreateBuffer(&desc, &data, &this->viewBuffer);
	if (FAILED(hr))
		return false;

	desc.ByteWidth = sizeof(DirectX::SimpleMath::Vector4);
	data.pSysMem = &this->eyePos;
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
}

void MainMenu::ZoomIn(DirectX::SimpleMath::Vector3 endPos)
{
}

void MainMenu::Update()
{
	Graphics::GetContext()->VSSetConstantBuffers(0, 1, &this->viewBuffer);
	Graphics::GetContext()->PSSetConstantBuffers(1, 1, &this->eyeBuffer);
}
