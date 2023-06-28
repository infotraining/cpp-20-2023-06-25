import Shapes;
import std;

int main()
{
	Shapes::ShapeFactory& shape_factory = Shapes::SingletonShapeFactory::instance();

	shape_factory.register_creator(Shapes::Rectangle::id, &std::make_unique<Shapes::Rectangle>);
	shape_factory.register_creator(Shapes::Square::id, &std::make_unique<Shapes::Square>);

	auto rect = shape_factory.create(Shapes::Rectangle::id);
	rect->draw();

	Shapes::Square sq{0, 200, 100};
	sq.draw();
	sq.move(50, 20);
	sq.draw();
}