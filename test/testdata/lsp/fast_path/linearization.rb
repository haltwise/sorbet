# typed: true
# frozen_string_literal: true

module Base
end

module BaseWithMethod
  include Base
  def foo

  end
end

module OtherModuleWithMethod
  def foo(i)

  end
end

class E
  include BaseWithMethod
  include OtherModuleWithMethod
end

E.new.foo(1)
