#include "core/TypePtr.h"
#include "core/Types.h"

using namespace std;

namespace sorbet::core {

void TypePtr::deleteTagged(Tag tag, void *ptr) noexcept {
    ENFORCE(ptr != nullptr);

    switch (tag) {
        case Tag::ClassType:
            delete reinterpret_cast<ClassType *>(ptr);
            break;

        case Tag::LambdaParam:
            delete reinterpret_cast<LambdaParam *>(ptr);
            break;

        case Tag::SelfTypeParam:
            delete reinterpret_cast<SelfTypeParam *>(ptr);
            break;

        case Tag::AliasType:
            delete reinterpret_cast<AliasType *>(ptr);
            break;

        case Tag::SelfType:
            delete reinterpret_cast<SelfType *>(ptr);
            break;

        case Tag::LiteralType:
            delete reinterpret_cast<LiteralType *>(ptr);
            break;

        case Tag::TypeVar:
            delete reinterpret_cast<TypeVar *>(ptr);
            break;

        case Tag::OrType:
            delete reinterpret_cast<OrType *>(ptr);
            break;

        case Tag::AndType:
            delete reinterpret_cast<AndType *>(ptr);
            break;

        case Tag::ShapeType:
            delete reinterpret_cast<ShapeType *>(ptr);
            break;

        case Tag::TupleType:
            delete reinterpret_cast<TupleType *>(ptr);
            break;

        case Tag::AppliedType:
            delete reinterpret_cast<AppliedType *>(ptr);
            break;

        case Tag::MetaType:
            delete reinterpret_cast<MetaType *>(ptr);
            break;

        case Tag::BlamedUntyped:
            delete reinterpret_cast<BlamedUntyped *>(ptr);
            break;

        case Tag::UnresolvedClassType:
            delete reinterpret_cast<UnresolvedClassType *>(ptr);
            break;

        case Tag::UnresolvedAppliedType:
            delete reinterpret_cast<UnresolvedAppliedType *>(ptr);
            break;
    }
}

bool TypePtr::isUntyped() const {
    auto *t = cast_type_const<ClassType>(*this);
    return t != nullptr && t->symbol == Symbols::untyped();
}

bool TypePtr::isNilClass() const {
    auto *t = cast_type_const<ClassType>(*this);
    return t != nullptr && t->symbol == Symbols::NilClass();
}

bool TypePtr::isBottom() const {
    auto *t = cast_type_const<ClassType>(*this);
    return t != nullptr && t->symbol == Symbols::bottom();
}

int TypePtr::typeKind() const {
    switch (tag()) {
        case Tag::AppliedType:
            return 1;
        case Tag::BlamedUntyped:
        case Tag::UnresolvedAppliedType:
        case Tag::UnresolvedClassType:
        case Tag::ClassType:
            return 2;
        case Tag::LiteralType:
            return 3;
        case Tag::ShapeType:
            return 4;
        case Tag::TupleType:
            return 5;
        case Tag::LambdaParam:
        case Tag::SelfTypeParam:
            return 6;
        case Tag::MetaType:
            return 7;
        case Tag::TypeVar:
            return 8;
        case Tag::AliasType:
            return 9;
        case Tag::OrType:
            return 10;
        case Tag::AndType:
            return 11;
        case Tag::SelfType:
            return 12;
    }
}

std::string TypePtr::typeName() const {
    switch (tag()) {
        case Tag::AppliedType:
            return "AppliedType";
        case Tag::BlamedUntyped:
            return "BlamedUntyped";
        case Tag::UnresolvedAppliedType:
            return "UnresolvedAppliedType";
        case Tag::UnresolvedClassType:
            return "UnresolvedClassType";
        case Tag::ClassType:
            return "ClassType";
        case Tag::LiteralType:
            return "LiteralType";
        case Tag::ShapeType:
            return "ShapeType";
        case Tag::TupleType:
            return "TupleType";
        case Tag::LambdaParam:
            return "LambdaParam";
        case Tag::SelfTypeParam:
            return "SelfTypeParam";
        case Tag::MetaType:
            return "MetaType";
        case Tag::TypeVar:
            return "TypeVar";
        case Tag::AliasType:
            return "AliasType";
        case Tag::OrType:
            return "OrType";
        case Tag::AndType:
            return "AndType";
        case Tag::SelfType:
            return "SelfType";
    }
}

bool TypePtr::isFullyDefined() const {
    switch (tag()) {
        // Base cases.
        case Tag::UnresolvedAppliedType:
        case Tag::UnresolvedClassType:
        case Tag::BlamedUntyped:
        case Tag::ClassType:
        case Tag::LiteralType:
        case Tag::AliasType:
        case Tag::SelfTypeParam:
        case Tag::MetaType: // MetaType: this is kinda true but kinda false. it's false for subtyping but true for
                            // inferencer.
            return true;

        case Tag::TypeVar:
        case Tag::LambdaParam:
        case Tag::SelfType:
            return false;

        // Composite types
        case Tag::ShapeType: {
            auto *shape = cast_type_const<ShapeType>(*this);
            return absl::c_all_of(shape->values, [](const TypePtr &t) { return t.isFullyDefined(); });
        }
        case Tag::TupleType: {
            auto *tuple = cast_type_const<TupleType>(*this);
            return absl::c_all_of(tuple->elems, [](const TypePtr &t) { return t.isFullyDefined(); });
        }
        case Tag::AndType: {
            auto *andType = cast_type_const<AndType>(*this);
            return andType->left.isFullyDefined() && andType->right.isFullyDefined();
        }
        case Tag::OrType: {
            auto *orType = cast_type_const<OrType>(*this);
            return orType->left.isFullyDefined() && orType->right.isFullyDefined();
        }
        case Tag::AppliedType: {
            auto *app = cast_type_const<AppliedType>(*this);
            for (auto &targ : app->targs) {
                if (!targ.isFullyDefined()) {
                    return false;
                }
            }
            return true;
        }
    }
}

} // namespace sorbet::core